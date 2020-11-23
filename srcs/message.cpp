#include "ircserv.hpp"
#include "message.hpp"

std::string
get_reply(IRCserv *serv, std::string _error, int fd, std::string const &command, const std::string &message)
{
	std::string	reply;
	std::string nickname;

	if (fd > 0)
	{
		std::vector<Client>::iterator	b = serv->clients.begin();
		std::vector<Client>::iterator   e = serv->clients.end();

		while (b != e)
		{
			if (b->getFD() == fd)
				break ;
			b++;
		}
		if (b != e)
			nickname = b->getnickname();
	}
	else
		nickname = "*";
	reply = ":" + serv->servername + " " + _error;
	if (_error != "")
		reply += " ";
	reply += nickname;
	if (!command.empty())
		reply += " " + command;
	if (!message.empty())
		reply += " :" + message;
	reply += CRLF;
	return reply;
}

void	msg_error(std::string msg, std::string msg1, IRCserv *serv)
{
	msg += msg1;
	msg_error(msg, serv);
}

void	msg_error(std::string msg, IRCserv *serv)
{
	std::map<int, t_fd>::iterator	b = serv->fds.begin();
	std::map<int, t_fd>::iterator	e = serv->fds.end();
	std::string	_msg("ERROR :");

	_msg += msg;
	_msg += CRLF;
	while (b != e)
	{
		if (b->second.type == FD_OPER)
			serv->fds[b->first].wrbuf += _msg;
		b++;
	}
}
