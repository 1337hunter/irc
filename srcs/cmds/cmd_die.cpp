#include "ircserv.hpp"
#include "message.hpp"

extern bool    g_server_die;

void	cmd_die(int fd, t_strvect const &split, IRCserv *serv)
{
	(void)split;
	if (serv->fds[fd].type == FD_OPER)
	{
		msg_error(serv->servername + " - OMG I'm dead! I'm DEAD!", serv);
		g_server_die = true;
	}
	else
		if (serv->fds[fd].type != FD_SERVER)
			serv->fds[fd].wrbuf += get_reply(serv, ERR_NOPRIVILEGES, fd, "",
					"Permission Denied- You're not an IRC operator");
}
