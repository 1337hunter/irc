#include "message.hpp"
#include "ircserv.hpp"
#include "tools.hpp"

void	cmd_admin(int fd, const t_strvect &split, IRCserv *serv)
{
	t_citer		client;
	std::string	forward_message;
	std::list<t_server>::iterator nb = serv->network.begin();
	std::list<t_server>::iterator ne = serv->network.end();

	client = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (client == serv->clients.end() && serv->fds[fd].type != FD_SERVER)
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1, "",
				"You have not registered");
	else if (split.size() == 1 ||
			(split.size() == 2 && match(serv->servername.c_str(), split[1].c_str())) ||
			(split.size() == 3 && match(serv->servername.c_str(), split[2].c_str())))
	{
		//if client sent "ADMIN" or "ADMIN <this servername>
		//or ":NICK" "ADMIN" "this servername" and servername matched
		if (split.size() == 3)
		{
			t_citer nick;
			std::string	str_nick(split[0], 1);

			nick = ft_findnick(serv->clients.begin(), serv->clients.end(), str_nick);
			if (nick == serv->clients.end())
			{
				serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHNICK, fd, "",
						str_nick + " :No such nick/channel");
				return ;
			}
			else
				fd = nick->getFD();
		}
		serv->fds[fd].wrbuf += get_reply(serv, RPL_ADMINME, fd, "",
				"Administrative info about " + serv->servername);
		serv->fds[fd].wrbuf += get_reply(serv, RPL_ADMINLOC1, fd, "",
				serv->admin.realname);
		serv->fds[fd].wrbuf += get_reply(serv, RPL_ADMINLOC2, fd, "",
				serv->admin.nick);
		serv->fds[fd].wrbuf += get_reply(serv, RPL_ADMINEMAIL, fd, "",
				serv->admin.mail);
	}
	else if (split.size() == 2)
	{
		//this message always must be sent by client
		while (nb != ne)
		{
			if (match(nb->servername.c_str(), split[1].c_str()))
				break ;
			nb++;
		}
		if (nb == ne)
			serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHSERVER, fd, "",
					split[1] + " " + ":No such server");
		else
		{
			forward_message = ":" + client->getnick() + " " + split[0] +
				" " + split[1] + CRLF;
			nb = serv->network.begin();
			while (nb != ne)
			{
				if (nb->hopcount == 1)
					serv->fds[nb->fd].wrbuf += forward_message;
				nb++;
			}
		}
	}
	else if (split.size() == 3)
	{
		//this message is sent by another server like forward message
		//or this server user
		forward_message = split[0] + " " + split[1] + " " + split[2] + CRLF;
		while (nb != ne)
		{
			if (nb->fd != fd && nb->hopcount == 1)
				serv->fds[nb->fd].wrbuf += forward_message;
			nb++;
		}
	}
}
