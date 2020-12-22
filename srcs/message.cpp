#include "ircserv.hpp"
#include "message.hpp"

std::string
get_reply(IRCserv *serv, std::string _error, int fd, std::string const &command, const std::string &message)
{
	std::string	reply;
	std::string nickname;

	if (fd > 0)
	{
		std::list<Client>::iterator	b = serv->clients.begin();
		std::list<Client>::iterator   e = serv->clients.end();

		while (b != e)
		{
			if (b->getFD() == fd)
				break ;
			b++;
		}
		if (b != e)
			nickname = b->getnick();
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

std::string
get_reply(IRCserv *serv, std::string rpl, Client *client, std::string command, std::string message)
{
	std::string	reply;

	reply = ":" + serv->servername + " " + (rpl.empty() ? "" : rpl + " ");
	reply += (client == 0 ? "*" : client->getnick());
	reply += (command.empty() ? "" : " " + command);
	reply += (message.empty() ? "" : " :" + message) + CRLF;
	return reply;
}

void	msg_error(std::string msg, std::string msg1, IRCserv *serv)
{
	msg += msg1;
	msg_error(msg, serv);
}

void	msg_error(std::string msg, IRCserv *serv)
{
	std::unordered_map<int, t_fd>::iterator	b = serv->fds.begin();
	std::unordered_map<int, t_fd>::iterator	e = serv->fds.end();
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

void	msg_forward(int fd, std::string const &msg, IRCserv *serv)
{
	std::vector<t_server>::iterator	net;

	for (net = serv->network.begin(); net != serv->network.end(); net++)
		if (fd != net->fd && !net->_blocked)
			serv->fds[net->fd].wrbuf += msg + CRLF;
}

void	msg_to_channel(Channel *channel, Client *client, std::string const &msg, IRCserv *serv, bool all)
{
	std::string	info;
	std::unordered_map<Client*, client_flags>::const_iterator   client_it;

	if (channel->isBlocked())
		return ;
	info = channel->getflags()._anonymous ? ":anonymous!anonymous@anonymous " : ":" + client->getinfo() + " ";
	client_it = channel->getclients().begin();
	for (; client_it != channel->getclients().end(); client_it++)
		if (client != client_it->first || all)
		{
			if (client_it->first->gethop() == 0)
				serv->fds[client_it->first->getFD()].wrbuf += info + msg + CRLF;
			else if (!client_it->first->isBlocked())
				serv->fds[client_it->first->getFD()].wrbuf += ":"+
					client->getnick() + " " + msg + CRLF;
		}
}

void	msg_to_channel_this(Channel *channel, Client *client, std::string msg, IRCserv *serv)
{
	std::unordered_map<Client*, client_flags>::const_iterator	client_it;
	std::string	info;

	if (channel->isBlocked())
		return ;
	info = channel->getflags()._anonymous ? ":anonymous!anonymous@anonymous " :
		client->getinfo() + " ";
	client_it = channel->getclients().begin();
	for (; client_it != channel->getclients().end(); client_it++)
		if (client_it->first->gethop() == 0)
			serv->fds[client_it->first->getFD()].wrbuf += info + msg + CRLF;
}

void	msg_each_client(std::string const &msg, Client *client, IRCserv *serv)
{
	std::list<Client>::iterator	client_it;

	for (client_it = serv->clients.begin(); client_it != serv->clients.end(); client_it++)
		if (client_it->getFD() != client->getFD() && !client_it->isBlocked())
			serv->fds[client_it->getFD()].wrbuf += ":" + client->getinfo() + " PRIVMSG " +
			client_it->getnick() + " " + msg + CRLF;
}
