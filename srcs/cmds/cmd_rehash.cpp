#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"

extern bool	g_rehash;

void	cmd_rehash(int fd, t_strvect const &split, IRCserv *serv)
{
	Client	*client;

	if (!(client = find_client_by_fd(fd, serv)) || !client->isRegistred())
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1, "",
				"You have not registered"); return ;
	}
	if (serv->fds[fd].type != FD_OPER)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOPRIVILEGES, client, "",
				"Permission Denied- You're not an IRC operator"); return ;
	}
	g_rehash = true;
	serv->fds[fd].wrbuf += get_reply(serv, RPL_REHASHING, client, "ircserv.conf",
			"Rehashing");
	msg_error("Server " + serv->servername + " is rehashing!", serv);
	(void)split;
}
