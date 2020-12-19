#include "ircserv.hpp"
#include "tools.hpp"
#include "commands.hpp"
#include "message.hpp"

//Command: PART
//Parameters: <channel> *( "," <channel> ) [ <Part Message> ]



void	part_from_network(int fd, t_strvect const &split, IRCserv *serv)
{
	Client		*client;
	t_strvect	args;
	Channel		*channel;
	std::string	part_msg;

	client = find_client_by_nick(get_nick_from_info(split[0]), serv);
	if (client == 0)
		return ;
	args = ft_splitstring(split[2], ',');
	if (split.size() > 3)
		part_msg = " " +  strvect_to_string(split, ' ', 3);
	else
		part_msg = " :Gone away";
	for (size_t i = 0; i < args.size(); i++)
	{
		channel = client->eraseChannel(args[i]);
		if (channel != 0)
		{
			if (channel->getclients().size() == 0)
				remove_channel(channel, serv);
			else
				msg_to_channel_this(channel, client, "PART " + args[i] + part_msg, serv);
		}
	}
	msg_forward(fd, strvect_to_string(split), serv);
}

void	part_from_client(int fd, const t_strvect &split, IRCserv *serv)
{
	Client		*client;
	t_strvect	args;
	t_strvect	forward_args;
	Channel		*channel;
	std::string	forward_msg;
	std::string	part_msg;

	if (!(client = find_client_by_fd(fd, serv)) || !client->isRegistred()) {
		serv->fds[fd].wrbuf += ":" + serv->servername +
			" 451 * :You have not registered" + CRLF;
		return ;
	}
	args = ft_splitstring(split[1], ',');
	if (split.size() > 2)
		part_msg = " " + strvect_to_string(split, ' ', 2);
	else
		part_msg = " :Gone away";
	for (size_t i = 0; i < args.size(); i++)
	{
		channel = client->eraseChannel(args[i]);
		if (channel != 0)
		{
			if (args.size() > 0 && args[i][0] != '&')
				forward_args.push_back(args[i]);
			if (channel->getclients().size() == 0)
				remove_channel(channel, serv);
			else
				msg_to_channel_this(channel, client, "PART " + args[i] + part_msg, serv);

		}
		else
		{
			if ((channel = find_channel_by_name(args[i], serv)) == 0 ||
				channel->getflags()._secret || channel->getflags()._anonymous ||
				channel->getflags()._private)
				serv->fds[fd].wrbuf += get_reply(serv, "403", fd, args[i],
				"No such channel");
			else
				serv->fds[fd].wrbuf += get_reply(serv, "442", fd, args[i],
				"You're not on that channel");
		}
	}
	forward_msg = ":" + client->getnickname() + " PART ";
	forward_msg += strvect_to_string(forward_args, ',') + part_msg;
	msg_forward(fd, forward_msg, serv);
}

void	cmd_part(int fd, t_strvect const &split, IRCserv *serv)
{
	if (split.size() < 2 && serv->fds[fd].type != FD_SERVER) {
		serv->fds[fd].wrbuf += get_reply(serv, "461", fd, "PART", "Not enough parameters");
		return ;
	}
	if (split.size() < 3 && serv->fds[fd].type == FD_SERVER)
		return ;
	if (serv->fds[fd].type == FD_SERVER)
		part_from_network(fd, split, serv);
	else
		part_from_client(fd, split, serv);
}
