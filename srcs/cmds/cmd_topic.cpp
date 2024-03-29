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
	channel->settopic(split[3]);
	msg_to_channel_this(channel, client, split[1] + " " + split[2] + " :" + split[3], serv);
	msg_forward(fd, split[0] + " " + split[1] + " " + split[2] + " :" + split[3], serv);
}

void	topic_from_client(int fd, t_strvect const &split, IRCserv *serv)
{
	Channel	*channel;
	Client	*client;
	std::string	topic_msg;

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
		topic_msg = " :" + split[2];
		if (channel->getflags()._topic_settable_by_chop)
			if (!channel->isOperator(client))
			{
				serv->fds[fd].wrbuf += get_reply(serv, ERR_CHANOPRIVSNEEDED, client,
					split[1], "You're not channel operator"); return ;
			}
		channel->settopic(std::string(split[2]));
		msg_to_channel_this(channel, client,
				split[0] + " " + split[1] + topic_msg, serv);
		msg_forward(fd, ":" + client->getnick() + " " + split[0] + " " + split[1] +
				topic_msg, serv);
	}
}

void	cmd_topic(int fd, t_strvect const &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_SERVER)
		topic_from_network(fd, split, serv);
	else
		topic_from_client(fd, split, serv);
}
