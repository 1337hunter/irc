#include "ircserv.hpp"
#include "message.hpp"
#include "tools.hpp"

void	list_from_network(int fd, t_strvect const &split, IRCserv *serv)
{
	Client		*client;
	t_strvect   args;
	Channel		*channel;
	t_server	*_serv;
	std::list<Channel>::iterator    chan;

	if (split.size() < 3)
		return ;
	if (!(client = find_client_by_nick(get_nick_from_info(split[0]), serv)))
		return ;
	if (split.size() == 4)
	{
		if (match(serv->servername, split[3]))
		{
			args = ft_splitstring(split[2], ',');
			for (size_t i = 0; i < args.size(); ++i)
				if ((channel = find_channel_by_name(args[i], serv)))
				{
					if (channel->getflags()._secret && !channel->isOnChan(client))
						continue ;
					else
						serv->fds[fd].wrbuf += get_reply(serv, RPL_LIST, client,
					channel->getname() + " " + channel->size(), channel->gettopic());
				}
			serv->fds[fd].wrbuf += get_reply(serv, RPL_LISTEND, client, "", "End of LIST");
		}
		else
			if ((_serv = find_server_by_mask(split[3], serv)))
				serv->fds[_serv->fd].wrbuf += strvect_to_string(split) + CRLF;
	}
	else if (split.size() == 3)
	{
		if (match(serv->servername, split[2]))
		{
			chan = serv->channels.begin();
			for (; chan != serv->channels.end(); chan++)
			{
				if (chan->getflags()._secret && !chan->isOnChan(client))
					continue ;
				else
					serv->fds[fd].wrbuf += get_reply(serv, RPL_LIST, client,
					chan->getname() + " " + chan->size(), chan->gettopic());
			}
			serv->fds[fd].wrbuf += get_reply(serv, RPL_LISTEND, client, "", "End of LIST");
		}
		else
			if ((_serv = find_server_by_mask(split[2], serv)))
				serv->fds[_serv->fd].wrbuf += strvect_to_string(split) + CRLF;
	}
}

void	list_from_client(int fd, t_strvect const &split, IRCserv *serv)
{
	Client		*client;
	t_strvect	args;
	Channel		*channel;
	t_server	*_serv;
	std::list<Channel>::iterator	chan;

	if (!(client = find_client_by_fd(fd, serv)) || !client->isRegistred())
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1, "",
				"You have not registered"); return ;
	}
	if (split.size() == 1)
	{
this_server:
		chan = serv->channels.begin();
		for (; chan != serv->channels.end(); chan++)
		{
			if (chan->getflags()._secret && !chan->isOnChan(client))
				continue ;
			else
			{
				serv->fds[fd].wrbuf += get_reply(serv, RPL_LIST, client, chan->getname() +
				" " + chan->size(), chan->gettopic());
			}
		}
		serv->fds[fd].wrbuf += get_reply(serv, RPL_LISTEND, client, "", "End of /LIST");
	}
	else if (split.size() == 2)
	{
		if (split[1].find_first_of("!#&+") != 0)
		{
			if (match(serv->servername, split[1]))
				goto this_server;
			else if (!(_serv = find_server_by_mask(split[1], serv)))
			{
				serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHSERVER, client, split[1],
						"No such server"); return ;
			}
			serv->fds[_serv->fd].wrbuf += ":" + client->getnick() + " " +
				strvect_to_string(split) + CRLF;
		}
		else
		{
this_server_with_args:
			args = ft_splitstring(split[1], ',');
			for (size_t i = 0; i < args.size(); ++i)
				if ((channel = find_channel_by_name(args[i], serv)))
				{
					if (channel->getflags()._secret && !chan->isOnChan(client))
						continue ;
					else
						serv->fds[fd].wrbuf += get_reply(serv, RPL_LIST, client,
						channel->getname() + " " + channel->size(),
						channel->gettopic());
				}
			serv->fds[fd].wrbuf += get_reply(serv, RPL_LISTEND, client, "", "End of LIST");
	
		}
	}
	else if (split.size() == 3)
	{
		if (match(serv->servername, split[2]))
			goto this_server_with_args;
		if (!(_serv = find_server_by_mask(split[2], serv)))
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHSERVER, client, split[2],
					"No such server"); return ;
		}
		serv->fds[_serv->fd].wrbuf += ":" + client->getnick() + " " +
			strvect_to_string(split) + CRLF;
	}
}

void	cmd_list(int fd, t_strvect const &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_SERVER)
		list_from_network(fd, split, serv);
	else
		list_from_client(fd, split, serv);
}
