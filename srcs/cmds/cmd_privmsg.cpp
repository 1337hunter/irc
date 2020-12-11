#include "ircserv.hpp"
#include "message.hpp"

//Command: PRIVMSG
//Parameters: <receiver>{,<receiver>} <text to be sent>
//

void	privmsg_from_network(int fd, t_strvect const &split, IRCserv *serv)
{
	fd = 0; (void)split; serv = 0;
}

void	privmsg_from_client(int fd, t_strvect const &split, IRCserv *serv)
{
	//Client	*client;
	//Channel	*channel;

	if (split.size() < 2)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NORECIPIENT, fd, "",
				"No recipient given (PRIVMSG)"); return ;
	}
	if (split.size() < 3)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTEXTTOSEND, fd, "",
				"No text to send"); return ;
	}
	if (split[1][0] == '$')
	{

	}
	else if (split[1][0] == '#' || split[1][0] == '!' || split[1][0] == '+' ||
			split[1][0] == '&')
	{

	}
	else
	{
	}
}

void	cmd_privmsg(int fd, t_strvect const &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_SERVER)
		privmsg_from_network(fd, split, serv);
	else
		privmsg_from_client(fd, split, serv);
}
