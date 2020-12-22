#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

void		remove_introduced_server(int fd, const t_strvect &split, IRCserv *serv)
{
	std::vector<t_server>::iterator serv_in_network = serv->network.begin();
	std::list<t_server_intro>::iterator	serv_intro;

	while (serv_in_network != serv->network.end())
	{
		serv_intro = serv_in_network->routing.begin();
		while (serv_intro != serv_in_network->routing.end())
		{
			if (serv_intro->behind == split[2])
			{
				std::unordered_map<int, t_fd>::iterator   mit = serv->fds.begin();
				std::string	forward;

				forward = strvect_to_string(split);
				serv_in_network->routing.erase(serv_intro);
				while (mit != serv->fds.begin())
				{
					if (mit->second.type == FD_SERVER && mit->first != fd)
						serv->fds[mit->first].wrbuf += forward;
					mit++;
				}
			}
		}
	}
}

void	squit_from_network(int fd, const t_strvect &split, IRCserv *serv)
{
	if (split.size() < 3)
		return ;
	remove_introduced_server(fd, split, serv);
}

void	squit_from_client(int fd, const t_strvect &split, IRCserv *serv)
{
	Client	*client = 0;
	t_server	*_serv;
	std::string	squit_msg;

	if (serv->fds[fd].type != FD_OPER && serv->fds[fd].type != FD_ME)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOPRIVILEGES, fd, "",
				"Permission Denied- You're not an IRC operator"); return ;
	}
	if (split.size() < 2 && serv->fds[fd].type == FD_OPER)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, fd, "SQUIT",
			"Not enough parameters"); return ;
	}
	if (serv->fds[fd].type == FD_OPER)
		if (!(client = find_client_by_fd(fd, serv)))
			return ;
	if (split.size() == 2)
		squit_msg = ":Network split";
	else if (split.size() > 2)
		squit_msg = split[2];
	if (!(_serv = find_server_by_name(split[1], serv)))
	{
		serv->fds[fd].wrbuf+= get_reply(serv, ERR_NOSUCHSERVER, fd, split[1],
			"No such server"); return ;
	}
	else if (_serv->servername != split[1] && serv->fds[fd].type == FD_OPER)
	{
		serv->fds[_serv->fd].wrbuf += ":" + client->getnick() +
			strvect_to_string(split) + CRLF; return ;
	}
	else if (_serv->servername == split[1])
	{
		std::list<Client>::iterator	client_it;
		std::list<Channel>::iterator	chan;
		std::list<blocked>::iterator	blocked_it;
		blocked						temp;

		temp.servername = split[1];
		client_it = _serv->clients.begin();
		for (; client_it != _serv->clients.end(); client_it++)
		{
			client_it->block();
			temp.clients.push_back(&(*client_it));
		}
		chan = serv->channels.begin();
		for (; chan != serv->channels.end(); chan++)
			if (chan->block_if())
			{
				blocked_it = serv->unavailable.begin();
				for (; blocked_it != serv->unavailable.end(); blocked_it++)
					if (blocked_it->servername == split[1])
						break ;
				if (blocked_it == serv->unavailable.end())
					temp.channels.push_back(&(*chan));
			}
		temp._blocked_time = ft_getcurrenttime();
		serv->unavailable.push_back(temp);
		if (serv->fds[fd].type == FD_OPER)
			serv->fds[_serv->fd].wrbuf += ":" + client->getnick() + " " +
					strvect_to_string(split) + CRLF;
		serv->fds[_serv->fd].status = false;
	}
}

void	cmd_squit(int fd, const t_strvect &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_UNREGISTRED)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1, "",
				"You have not registered"); return ;
	}
	if (serv->fds[fd].type == FD_SERVER && split[0][0] == ':')
		squit_from_network(fd, split, serv);
	else
		squit_from_client(fd, split, serv);
}
