#include "ircserv.hpp"
#include "message.hpp"
#include "tools.hpp"

void	cmd_wallops(int fd, t_strvect const &split, IRCserv *serv)
{
	std::list<Client>::iterator		client;
	std::string	msg;

	if (split.size() != 3)
		return ;
	msg = split[0] + " " + split[2] + CRLF;
	client = serv->clients.begin();
	for (; client != serv->clients.end(); client++)
		if (client->isWallOps())
			serv->fds[client->getFD()].wrbuf += msg;
	msg_forward(fd, strvect_to_string(split), serv);
}
