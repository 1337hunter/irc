#include "tools.hpp"
#include "ircserv.hpp"

void    cmd_njoin(int fd, const t_strvect &split, IRCserv *serv)
{
	if (serv->fds[fd].type != FD_SERVER && serv->fds[fd].type != FD_ME)
	{
		serv->fds[fd].wrbuf += "ERROR :This command only for server-server communication" +
			std::string(CRLF); return ;
	}
	(void)split;
}
