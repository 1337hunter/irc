#include "ircserv.hpp"
#include "message.hpp"
#include "tools.hpp"

void	topic_from_network(int fd, t_strvect const &split, IRCserv *serv)
{
	Client	*client;
	Channel	*channel;

	if (split.size() < 4)
		return ;
	if (!(client = find_client_by_nick(get_nick_from_info(split[0]), serv)))
		return ;
	if (!(channel = find_channel_by_name(split[2], serv)))
		return ;
	channel->settopic(std::string(split[3]));
	msg_to_channel_this(channel, client, strvect_to_string(split, ' ', 1), serv);
	msg_forward(fd, strvect_to_string(split), serv);
}

void	topic_from_client(int fd, t_strvect const &split, IRCserv *serv)
{
	Channel	*channel;
	Client	*client;

	if (!(client = find_client_by_fd(fd, serv)) || !client->isRegistred())
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1, "",
			"You have not registered"); return ;
	}
	if (split.size() < 2)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, client, "TOPIC",
			"Not enough parameters"); return ;
	}
	if (!(channel = find_channel_by_name(split[1], serv)))
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHCHANNEL, client, split[1],
				"No such channel"); return ;
	}
	if (!channel->isOnChan(client))
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTONCHANNEL, client, split[1],
				"You're not on that channel"); return ;
	}
	if (split.size() == 2)
	{
		if (channel->gettopic().empty())
			serv->fds[fd].wrbuf += get_reply(serv, RPL_NOTOPIC, client, split[1],
				"No topic is set");
		else
			serv->fds[fd].wrbuf += get_reply(serv, RPL_TOPIC, client, split[1],
					channel->gettopic());
		return ;
	}
	else
	{
		if (channel->getflags()._topic_settable_by_chop)
			if (!channel->isOperator(client))
			{
				serv->fds[fd].wrbuf += get_reply(serv, ERR_CHANOPRIVSNEEDED, client,
					split[1], "You're not channel operator"); return ;
			}
		channel->settopic(std::string(split[2]));
		msg_to_channel_this(channel, client, strvect_to_string(split), serv);
		msg_forward(fd, ":" + client->getnick() + " " + strvect_to_string(split), serv);
	}
}

void	cmd_topic(int fd, t_strvect const &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_SERVER)
		topic_from_network(fd, split, serv);
	else
		topic_from_client(fd, split, serv);
}
