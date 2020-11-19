#include "message.hpp"
#include "ircserv.hpp"

void	cmd_admin(int fd, const t_strvect &split, IRCserv *serv)
{
	t_citer	fd_entry;

	fd_entry = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (fd_entry == serv->clients.end())
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1, "",
				"You have not registered");
	else if (split.size() == 1)
	{
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
	}
}
