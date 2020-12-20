#include "ircserv.hpp"
#include "tools.hpp"
#include "commands.hpp"
#include "message.hpp"

//Command: NAMES
//Parameters: [ <channel> *( "," <channel> ) [ <target> ] ]

void	names_from_network(int fd, t_strvect const &split, IRCserv *serv)
{
	Client	*client;
	Channel	*channel;
	t_strvect   chan_args;
	t_server	*_serv = 0;

	if (split.size() < 4)
		return ;
	if (!(client = find_client_by_nick(get_nick_from_info(split[0]), serv)))
		return ;
	if (match(serv->servername, split[3]))
	{
		chan_args = ft_splitstring(split[2], ',');
		for (size_t i = 0; i < chan_args.size(); i++)
			if ((channel = find_channel_by_name(chan_args[i], serv)))
			{
				if (channel->getflags()._anonymous ||
					((channel->getflags()._secret || channel->getflags()._private) &&
					!channel->isOnChan(client)))
						continue ;
				else
					serv->fds[fd].wrbuf += reply_chan_names(serv, channel, client);
			}
	}
	else
	{
		if (split[3].find_first_of("*?") == NPOS)
			_serv = find_server_by_name(split[3], serv);
		else
			_serv = find_server_by_mask(split[3], serv);
		if (!_serv)
			return ;
		serv->fds[_serv->fd].wrbuf += strvect_to_string(split) + CRLF;
	}
}

void	names_from_client(int fd, t_strvect const &split, IRCserv *serv)
{
	t_strvect	chan_args;
	std::list<Channel>::iterator    chan;
	std::list<Client>::iterator		client_it;
	std::list<Channel*>::iterator	client_chan;
	Client  *client;
	Channel	*channel;

	if (!(client = find_client_by_fd(fd, serv)) || !client->isRegistred()) {
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1 , "NAMES",
				"You have not registered");
		return ;
	}
	if (split.size() == 1)
	{
		for (chan = serv->channels.begin(); chan != serv->channels.end(); chan++)
		{
			client_chan = client->getchannels().begin();
			for (;client_chan != client->getchannels().end(); client_chan++)
				if ((*client_chan) == &(*chan))
					break ;
			if ((chan->getflags()._private || chan->getflags()._secret ||
					chan->getflags()._anonymous) &&
					(client->isOperator() || (*client_chan) != &(*chan)))
				continue ;
			else
				serv->fds[fd].wrbuf += reply_chan_names(serv, &(*chan), client);
		}
		serv->fds[fd].wrbuf += reply_nochan_visible_names(serv, client);
	}
	else if (split.size() == 2 && split[1].find_first_of("!#+&") == 0)
	{
this_server_names:
		chan_args = ft_splitstring(split[1], ',');
		for (size_t i = 0; i < chan_args.size(); i++)
			if ((channel = find_channel_by_name(chan_args[i], serv)))
			{
				if (channel->getflags()._anonymous ||
					((channel->getflags()._secret || channel->getflags()._private) &&
					!channel->isOnChan(client)))
						continue ;
				else
					serv->fds[fd].wrbuf += reply_chan_names(serv, channel, client);
			}
		return ;
	}
	else if (split.size() == 3)
	{
		if (match(serv->servername, split[2]))
			goto this_server_names;
		else
		{
			t_server	*_serv = 0;

			if (split[2].find_first_of("?*") == NPOS)
				_serv = find_server_by_name(split[2], serv);
			else
				_serv = find_server_by_mask(split[2], serv);
			if (!_serv)
			{
				serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHSERVER, client, split[2],
						"No such server"); return ;
			}
			serv->fds[_serv->fd].wrbuf += ":" + client->getnick() + " " +
				strvect_to_string(split) + CRLF;
		}
	}
}

void	cmd_names(int fd, const t_strvect &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_SERVER)
		names_from_network(fd, split, serv);
	else
		names_from_client(fd, split, serv);
}
