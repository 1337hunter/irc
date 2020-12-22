#include "ircserv.hpp"
#include "message.hpp"
#include "tools.hpp"

void	kick_from_client(int fd, t_strvect const &split, IRCserv *serv)
{
	Client		*client;
	Client		*client_kick;
	Channel		*channel;
	t_strvect	chan_arg;
	t_strvect	user_arg;

	if (!(client = find_client_by_fd(fd, serv)) || !client->isRegistred())
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1, "",
				"You have not registered"); return ;
	}
	if (split.size() < 3)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, client, "KICK",
				"Not enough parameters"); return ;
	}
	chan_arg = ft_splitstring(split[1], ',');
	user_arg = ft_splitstring(split[2], ',');
	if (chan_arg.size() > 1 && user_arg.size() != chan_arg.size())
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, client, "KICK",
				"Not enough parameters"); return ;
	}
	if (chan_arg.size() == 1)
	{
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
		if (!channel->isOperator(client))
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_CHANOPRIVSNEEDED, client, split[1],
					"You're not channel operator"); return ;
		}
		for (size_t i = 0; i < user_arg.size(); i++)
		{
			if (!(client_kick = find_client_by_nick(user_arg[i], serv)))
			{
				serv->fds[fd].wrbuf += get_reply(serv, ERR_USERNOTINCHANNEL, client,
				user_arg[i] + " " + split[1], "They aren't on that channel"); continue ;
			}
			if (!channel->isOnChan(client_kick))
				serv->fds[fd].wrbuf += get_reply(serv, ERR_USERNOTINCHANNEL, client,
						user_arg[i] + " " + split[1], "They aren't on that channel");
			else
			{
				msg_to_channel(channel, client_kick, "PART " + split[1] + " " +
					(split.size() > 3 ? ":" + split[3] : ":User has been kicked!"),
					serv, true);
				client_kick->eraseChannel(channel->getname());
			}
		}
	}
	else
	{
		for (size_t i = 0; i < chan_arg.size(); ++i)
		{
			if (!(channel = find_channel_by_name(chan_arg[i], serv)))
			{
				serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHCHANNEL, client,
					chan_arg[i], "No such channel"); continue ;
			}
			if (!(client_kick = find_client_by_nick(user_arg[i], serv)))
			{
				serv->fds[fd].wrbuf += get_reply(serv, ERR_USERNOTINCHANNEL, client,
				user_arg[i] + " " + chan_arg[i], "They aren't on that channel"); continue ;
			}
			if (!channel->isOnChan(client_kick))
				serv->fds[fd].wrbuf += get_reply(serv, ERR_USERNOTINCHANNEL, client,
					user_arg[i] + " " + chan_arg[i], "They aren't on that channel");
			else
			{
				msg_to_channel(channel, client_kick, "PART " + split[1] + " " +
					(split.size() > 3 ? ":" + split[3] : ":User has been kicked!"),
					serv, true);
				client_kick->eraseChannel(channel->getname());
			}
		}
	}
}

void	cmd_kick(int fd, t_strvect const &split, IRCserv *serv)
{
	if (serv->fds[fd].type != FD_SERVER)
		kick_from_client(fd, split, serv);
}
