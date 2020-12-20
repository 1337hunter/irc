#include "tools.hpp"
#include "ircserv.hpp"
#include "message.hpp"

void    cmd_njoin(int fd, const t_strvect &split, IRCserv *serv)
{
	t_strvect	clients;
	Channel		*channel;
	Client		*client;

	if (serv->fds[fd].type != FD_SERVER && serv->fds[fd].type != FD_ME)
	{
		serv->fds[fd].wrbuf += "ERROR :This command is only for server-server ";
		serv->fds[fd].wrbuf += "communication\r\n"; return ;
	}
	if (split.size() < 4)
		return ;
	clients = ft_splitstring(std::string(split[3], 1), ',');
	if (!(channel = find_channel_by_name(split[2], serv)))
	{
		serv->channels.push_back(Channel(split[2]));
		channel = &(serv->channels.back());
	}
	for (size_t i = 0; i < clients.size(); ++i)
		if (clients[i][0] == '@' && clients[i][1] == '@')
		{
			if ((client = find_client_by_nick(std::string(clients[i], 2), serv)))
			{
				channel->add_client(client, 1, 1, 0);
				msg_to_channel_this(channel, client, "JOIN :" + channel->getname(), serv);
			}
		}
		else if (clients[i][0] == '@' && clients[i][1] != '@')
		{
			if ((client = find_client_by_nick(std::string(clients[i], 1), serv)))
			{
				channel->add_client(client, 0, 1, 0);
				msg_to_channel_this(channel, client, "JOIN :" + channel->getname(), serv);
			}
		}
		else if (clients[i][0] == '+')
		{
			if ((client = find_client_by_nick(std::string(clients[i], 1), serv)))
			{
				channel->add_client(client, 0, 0, 1);
				msg_to_channel_this(channel, client, "JOIN :" + channel->getname(), serv);
			}
		}
		else
		{
			if ((client = find_client_by_nick(clients[i], serv)))
			{
				channel->add_client(client);
				msg_to_channel_this(channel, client, "JOIN :" + channel->getname(), serv);
			}
		}
	msg_forward(fd, strvect_to_string(split), serv);
}
