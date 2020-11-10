#include "ircserv.hpp"

std::string
get_reply(IRCserv *_server, std::string _error, int fd, std::string const &command, const std::string &message)
{
	std::string	reply;
	std::string nickname;

	nickname = "*";
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
