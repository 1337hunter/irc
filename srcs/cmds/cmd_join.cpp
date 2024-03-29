#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"
#include "commands.hpp"

//Command: JOIN
//Parameters: <channel>[ %x7 <modes> ] *( "," <channel>[ %x7 <modes> ] )
//
//Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"

void	join_backward(IRCserv *serv, std::list<Channel>::iterator chan, Client *client)
{
	std::MAP<Client*, client_flags>::iterator c_map;
	std::string		reply;
	std::string		forward;

	if (chan->getflags()._anonymous)
		reply = ":anonymous!anonymous@anonymous";
	else
		reply = ":" + client->getnick() + "!" + client->getusername() +
		"@" + client->gethostname();
	reply += " JOIN :" + chan->getname() + CRLF;
	for (c_map = chan->getclients().begin(); c_map != chan->getclients().end(); c_map++)
		if (c_map->first->getFD() != client->getFD() && c_map->first->gethop() == 0)
			serv->fds[c_map->first->getFD()].wrbuf += reply;
	serv->fds[client->getFD()].wrbuf += reply;
	serv->fds[client->getFD()].wrbuf += reply_chan_names(serv, &(*chan), client);
}

void	join_to_chan(int fd, const t_strvect &split, IRCserv *serv)
{
	std::list<Channel>::iterator	chan;
	std::vector<std::string>		args;
	std::vector<std::string>		keys;
	std::string						str_chan;
	size_t	i;
	bool	create = true;
	Client	*client;

	if (!(client = find_client_by_fd(fd, serv)) || !client->isRegistred())
    {
        serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1 , "JOIN",
                "You have not registered"); return ;
    }
	if (split[1][0] == ':')
		str_chan = std::string(split[1], 1);
	else
		str_chan = split[1];
	args = ft_splitstring(str_chan, ",");
	if (split.size() > 2)
		keys = ft_splitstring(split[2], ",");
	i = keys.size() - 1;
	while (++i < args.size())
		keys.push_back("");
	for (i = 0; i < args.size(); i++)
	{
		for (chan = serv->channels.begin(); chan != serv->channels.end(); chan++)
			if (chan->getname() == args[i])
			{
				create = false;
				if (chan->isOnChan(client))
						break ;
				if (chan->isBlocked())
				{
					serv->fds[fd].wrbuf += get_reply(serv, ERR_UNAVAILRESOURCE,
							fd, args[i], "Nick/channel is temporarily unavailable");
					break ;
				}
				if (keys[i] != chan->getkey())
				{
					serv->fds[fd].wrbuf += get_reply(serv, ERR_BADCHANNELKEY, fd,
						args[i], "Cannot join channel (+k)");
					break ;
				}
				if (chan->getflags()._quiet)
					if (!client->isOperator())
					{
						serv->fds[fd].wrbuf += get_reply(serv, "481", fd, args[i],
								"Permission Denied- You're not an IRC operator");
						break ;
					}
				if (chan->getflags()._invite_only)
					if (!client->isInvited(&(*chan)))
					{
						serv->fds[fd].wrbuf += get_reply(serv, "473", fd, args[i],
								"Cannot join channel (+i)");
						break ;
					}
				if (chan->getflags()._ban_mask.size() > 0)
				{
					if (chan->isBanned(client))
					{
						serv->fds[fd].wrbuf += get_reply(serv, "474", fd, args[i],
								"Cannot join channel (+b)");
						break ;
					}
				}
				if (chan->getflags()._limit_of_users <= chan->getclients().size()
						&& !client->isOperator())
				{
					serv->fds[fd].wrbuf += get_reply(serv, "471", fd, args[i],
							"Cannot join channel (+l)");
					break ;
				}
				chan->add_client(client);
				client->add_channel(chan->getptr());
				if (args[i][0] != '&')
					msg_forward(-1, ":" + client->getnick() + " JOIN " + args[i], serv);
				join_backward(serv, chan, client);
				break ;
			}
		if (create)
		{
			serv->channels.push_back(Channel(args[i], keys[i], client));
			client->add_channel((serv->channels.rbegin())->getptr());
			join_backward(serv, --serv->channels.end(), client);
			if (args[i][0] != '&')
				msg_forward(-1, ":" + client->getnick() + " JOIN " + args[i], serv);
			if (!keys[i].empty())
				msg_forward(-1, "MODE " + args[i] + " +k " + keys[i], serv);
		}
		create = true;
	}
}

void	join_from_network(int fd, const t_strvect &split, IRCserv *serv)
{
	Client		*client;
	Channel		*channel;
	t_strvect	chans;
	t_strvect	modes;
	std::string	str_chan;

	if (split.size() < 3)
		return ;
	if (split[2][0] == ':')
		str_chan = std::string(split[2], 1);
	else
		str_chan = split[2];
	if ((client = find_client_by_nick(get_nick_from_info(split[0]), serv)) == 0)
		return ;
	chans = ft_splitstring(str_chan, ',');
	for (size_t	i = 0; i < chans.size(); i++)
	{
		modes = ft_splitstring(chans[i], static_cast<char>(7));
		channel = find_channel_by_name(modes[0], serv);
		if (channel == 0)
		{
			if (modes.size() > 1)
				serv->channels.push_back(Channel(modes[0], client, modes[1]));
			else
				serv->channels.push_back(Channel(modes[0], client));
			channel = &serv->channels.back();
			client->add_channel(channel);
		}
		else
		{
			channel->add_client(client->getptr());
			client->add_channel(channel->getptr());
		}
		msg_to_channel_this(channel, client, "JOIN :" + channel->getname(), serv);
	}
	msg_forward(fd, strvect_to_string(split), serv);
}

void	cmd_join(int fd, t_strvect const &split, IRCserv *serv)
{
	if (split.size() < 2)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, fd, "JOIN",
				"Not enough parameters");
		return ;
	}
	if (serv->fds[fd].type == FD_CLIENT || serv->fds[fd].type == FD_OPER)
		join_to_chan(fd, split, serv);
	else if (split[0][0] == ':' && serv->fds[fd].type == FD_SERVER)
		join_from_network(fd, split, serv);
	else if (serv->fds[fd].type == FD_UNREGISTRED)
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1 , "JOIN",
				"You have not registered");
}
