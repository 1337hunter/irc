#include "ircserv.hpp"

//Command: PRIVMSG
//Parameters: <receiver>{,<receiver>} <text to be sent>
//

void	cmd_privmsg(int fd, t_strvect const &split, IRCserv *serv)
{
	fd = 0;
	(void)split;
	serv = 0;
}
