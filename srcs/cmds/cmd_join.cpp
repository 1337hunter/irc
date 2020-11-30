//Command: JOIN
//Parameters: <channel>[ %x7 <modes> ] *( "," <channel>[ %x7 <modes> ] )

#include "ircserv.hpp"

void	cmd_join(int fd, t_strvect const &split, IRCserv *serv)
{
	fd = 0;
	(void)split;
	serv = 0;
}
