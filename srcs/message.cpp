#include "ircserv.hpp"
#include "message.hpp"

std::string
get_reply(IRCserv *_server, std::string _error, int fd, std::string const &command, const std::string &message)
{
	std::string	reply;
	std::string nickname;

	if (fd > 0)
	{
		std::vector<Client>::iterator	b = _server->clients.begin();
		std::vector<Client>::iterator   e = _server->clients.end();

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
	reply = ":" + _server->hostname + " " + _error + " ";
	reply += nickname;
	if (!command.empty())
		reply += " " + command;
	if (!message.empty())
		reply += " :" + message;
	reply += CLRF;
	return reply;
}

void	msg_error(std::string msg, std::string msg1, IRCserv *_server)
{
	msg += msg1;
	msg_error(msg, _server);
}

void	msg_error(std::string msg, IRCserv *_server)
{
	std::map<int, t_fd>::iterator	b = _server->fds.begin();
	std::map<int, t_fd>::iterator e = _server->fds.end();

	std::string	_msg("ERROR :");
	_msg += msg;
	_msg += CLRF;
	while (b != e)
	{
		if (b->second.type == FD_SERVER || b->second.type == FD_OPER)
			_server->fds[b->first].wrbuf += _msg;
		b++;
	}
}
