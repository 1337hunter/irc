#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"

//Command: PRIVMSG
//Parameters: <receiver>{,<receiver>} <text to be sent>
//
//2812 :
//Command: PRIVMSG
//Parameters: <msgtarget> <text to be sent>

void	privmsg_from_network(int fd, t_strvect const &split, IRCserv *serv)
{
	Client		*client;
	t_server	*routing;
	Channel		*channel;
	Client		*client_msg;
	std::unordered_map<Client*, client_flags>::const_iterator   client_it;

	if (split.size() < 4)
		return ;
	if (!(client = find_client_by_nick(std::string(split[0], 1), serv)))
		return ;
	if (split[2][0] == '$')
	{
		if (std::string(split[2], 1) == serv->servername)
			msg_each_client(strvect_to_string(split, ' ', 3), client, serv);
		else
		{
			if (!(routing = find_server_by_name(std::string(split[2], 1), serv)))
				return ;
			serv->fds[routing->fd].wrbuf += strvect_to_string(split) + CRLF;
		}
	}
	else if (split[2].find_first_of("!#+") == 0)
	{
		if (!(channel = find_channel_by_name(split[2], serv)))
			return ;
		client_it = channel->getclients().begin();
		for (; client_it != channel->getclients().end(); client_it++)
			if (client_it->first->getFD() != fd)
			{
				if (client_it->first->gethop() == 0)
					serv->fds[client_it->first->getFD()].wrbuf += ":" + client->getinfo()
					+ " " + strvect_to_string(split, ' ', 1) + CRLF;
				else
					serv->fds[client_it->first->getFD()].wrbuf += strvect_to_string(split)
					+ CRLF;
			}
	}
	else
	{
		if (!(client_msg = find_client_by_nick(split[2], serv)))
			return ;
		if (client_msg->gethop() == 0 && client_msg->isAway())
			serv->fds[fd].wrbuf += get_reply(serv, RPL_AWAY, client, client_msg->getnick(),
			client_msg->getAwayMsg());
		else if (client_msg->gethop() == 0 && !client->isAway())
			serv->fds[client_msg->getFD()].wrbuf += ":" + client->getinfo() + " " +
			strvect_to_string(split, ' ', 1) + CRLF;
		else if (client_msg->getFD() != fd)
			serv->fds[client_msg->getFD()].wrbuf += strvect_to_string(split) + CRLF;
	}
}

void	privmsg_from_client(int fd, t_strvect const &split, IRCserv *serv)
{
	Client		*client;
	Client		*client_msg;
	Channel		*channel;
	bool		good_mask = false;
	std::vector<t_server>::iterator net;
	std::list<Client>::iterator		client_it;
	std::list<t_server_intro>::iterator	routing;
	t_server	*_serv = 0;
	bool		onChan = false;

	if (!(client = find_client_by_fd(fd, serv)) || !client->isRegistred())
	{
		serv->fds[fd].wrbuf += get_reply(serv, "451", -1, "", "You have not registered");
		return ;
	}
	if (split.size() < 2)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NORECIPIENT, client, "",
				"No recipient given (PRIVMSG)"); return ;
	}
	if (split.size() < 3)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTEXTTOSEND, client, "",
				"No text to send"); return ;
	}
	if (split[1].find_first_of("*?") != NPOS)
	{
		if (!is_valid_serv_host_mask(split[1]))
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_BADMASK, client, split[1],
					"Bad Server/host mask"); return ;
		}
		good_mask = true;
	}
	if (split[1][0] == '$') //server mask or servername
	{
		if (!client->isOperator())
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_NOPRIVILEGES, client, "",
			"Permission Denied- You're not an IRC operator"); return ;
		}
		if (good_mask)
		{
			net = serv->network.begin();
			for (; net != serv->network.end(); net++)
			{
				if (match(net->servername, std::string(split[1], 1)))
					serv->fds[net->fd].wrbuf += ":" + client->getnick() + " PRIVMSG $"
					+ net->servername + " " + strvect_to_string(split, ' ', 2) + CRLF;
				routing = net->routing.begin();
				for (; routing != net->routing.end(); routing++)
					if (match(routing->servername, std::string(split[1], 1)))
					{
						serv->fds[net->fd].wrbuf += ":" + client->getnick() +
						" PRIVMSG $" + routing->servername + " " +
						strvect_to_string(split, ' ', 2) + CRLF;
						break ;
					}
			}
			if (match(serv->servername, std::string(split[1], 1)))
				msg_each_client(strvect_to_string(split, ' ', 2), client, serv);
		}
		else
			_serv = find_server_by_name(std::string(split[1], 1), serv);
		if (_serv)
			serv->fds[fd].wrbuf += ":" + client->getnick() +
				" " + strvect_to_string(split) + CRLF;
		else if (std::string(split[1], 1) == serv->servername)
			msg_each_client(strvect_to_string(split, ' ', 2), client, serv);
		else if (!_serv && !good_mask)
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHSERVER, client, split[1],
					"No such server"); return ;
		}
	}
	else if (split[1][0] == '#' && good_mask) //host mask
	{
		if (!client->isOperator())
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_NOPRIVILEGES, client, "",
			"Permission Denied- You're not an IRC operator"); return ;
		}
		if (match(serv->servername, std::string(split[1], 1)))
			msg_each_client(strvect_to_string(split, ' ', 2), client, serv);
		for (net = serv->network.begin(); net != serv->network.end(); net++)
			for (client_it = net->clients.begin(); client_it != net->clients.end();
					client_it++)
				if (match(client_it->gethostname(), std::string(split[1], 1)))
					serv->fds[client_it->getFD()].wrbuf += ":" +
						client->getnick() + " PRIVMSG " + client_it->getnick() +
						strvect_to_string(split, ' ', 2);
	}
	else if (split[1].find_first_of("!#&+") == 0) //to channel
	{
		if (!(channel = find_channel_by_name(split[1], serv)))
		{
			serv->fds[fd].wrbuf += get_reply(serv, "403", client, split[1],
					"No such channel"); return ;
		}
		onChan = channel->isOnChan(client);
		if ((channel->getflags()._no_messages_outside && !onChan) ||
			channel->isBanned(client) || (channel->getflags()._moderated &&
			onChan && !channel->getclients()[client]._voice &&
			!channel->isOperator(client)) || (channel->getflags()._moderated && !onChan))
		{
			serv->fds[fd].wrbuf += get_reply(serv, "404", client, split[1],
					"Cannot send to channel"); return ;
		}
		if (split[1][0] == '&')
			msg_to_channel_this(channel, client, strvect_to_string(split), serv);
		else
			msg_to_channel(channel, client, strvect_to_string(split), serv);
	}
	else //to client
	{
		if (split[1].find_first_of("%!@") == NPOS)
			client_msg = find_client_by_nick(split[1], serv);
		else
			client_msg = find_client_by_user_or_nick_and_host(split[1], serv);
		if (!client_msg)
		{
			serv->fds[fd].wrbuf += get_reply(serv, "401", client, split[1],
					"No such nick/channel"); return ;
		}
		if (client_msg->gethop() == 0 && !client_msg->isAway())
			serv->fds[client_msg->getFD()].wrbuf += ":" + client->getinfo() + " PRIVMSG " +
			client_msg->getnick() + " " + strvect_to_string(split, ' ', 2) + CRLF;
		else if (client_msg->gethop() == 0 && client_msg->isAway())
			serv->fds[client->getFD()].wrbuf += get_reply(serv, RPL_AWAY, client,
			client_msg->getnick(), client_msg->getAwayMsg());
		else
			serv->fds[client_msg->getFD()].wrbuf += ":" + client->getnick() + " PRIVMSG " +
			client_msg->getnick() + " " + strvect_to_string(split, ' ', 2) + CRLF;
	}
}

void	cmd_privmsg(int fd, t_strvect const &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_SERVER)
		privmsg_from_network(fd, split, serv);
	else
		privmsg_from_client(fd, split, serv);
}
