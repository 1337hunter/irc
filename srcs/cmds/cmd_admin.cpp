#include "message.hpp"
#include "ircserv.hpp"
#include "tools.hpp"

void	admin_reply(int fd, Client *client, IRCserv *serv)
{
	serv->fds[fd].wrbuf += get_reply(serv, RPL_ADMINME, client, "",
		"Administrative info about " + serv->servername);
	serv->fds[fd].wrbuf += get_reply(serv, RPL_ADMINLOC1, client, "",
		serv->admin.realname);
	serv->fds[fd].wrbuf += get_reply(serv, RPL_ADMINLOC2, client, "",
		serv->admin.nick);
	serv->fds[fd].wrbuf += get_reply(serv, RPL_ADMINEMAIL, client, "",
		serv->admin.mail);
}

void	admin_from_network(int fd, const t_strvect &split, IRCserv *serv)
{
	Client	*client = 0;
	t_server	*_serv = 0;

	if (split.size() < 3)
		return ;
	if (!(client = find_client_by_nick(get_nick_from_info(split[0]), serv)))
		return ;
	if (match(serv->servername, split[2]))
		admin_reply(fd, client, serv);
	else
	{
		if (!(_serv = find_server_by_mask(split[2], serv)))
			return ;
		serv->fds[_serv->fd].wrbuf += split[0] + " " + split[1] + " :" + split[2] + CRLF;
	}
}

void	admin_from_client(int fd, const t_strvect &split, IRCserv *serv)
{
	Client	*client = 0;
	t_server	*_serv = 0;

	if (!(client = find_client_by_fd(fd, serv)))
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1, "",
				"You have not registered"); return ;
	}
	if (split.size() == 1)
	{
admin_this_server:
		admin_reply(fd, client, serv);
	}
	else if (split.size() > 1)
	{
		if (match(serv->servername, split[1]))
            goto admin_this_server;
		if (!(_serv = find_server_by_mask(split[1], serv)))
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHSERVER, fd, "",
				split[1] + " " + ":No such server"); return ;
		}
		serv->fds[_serv->fd].wrbuf += ":" + client->getnick() + " " + split[0] + " " +
				split[1] + CRLF;
	}
}

void	cmd_admin(int fd, const t_strvect &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_SERVER)
		admin_from_network(fd, split, serv);
	else
		admin_from_client(fd, split, serv);
}
