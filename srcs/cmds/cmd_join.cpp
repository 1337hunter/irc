/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_join.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/01 15:42:46 by gbright           #+#    #+#             */
/*   Updated: 2020/12/08 15:18:32 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"
#include "commands.hpp"

//Command: JOIN
//Parameters: <channel>[ %x7 <modes> ] *( "," <channel>[ %x7 <modes> ] )
//
//Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"

void	join_backward(IRCserv *serv, std::list<Channel>::iterator chan, t_citer client_it)
{
	std::unordered_map<Client*, client_flags>::iterator c_map;
	std::string		reply;

	if (chan->getflags()._anonymous)
		reply = ":anonymous!anonymous@anonymous";
	else
		reply = ":" + client_it->getnickname() + "!" + client_it->getusername() +
		"@" + client_it->gethostname();
	reply += " JOIN :" + chan->getname() + CRLF;
	for (c_map = chan->getclients().begin(); c_map != chan->getclients().end(); c_map++)
		if (c_map->first->getFD() != client_it->getFD())
			serv->fds[c_map->first->getFD()].wrbuf += reply;
	serv->fds[client_it->getFD()].wrbuf += reply;
	serv->fds[client_it->getFD()].wrbuf += reply_chan_names(serv, chan, &(*client_it));
}

void	join_to_chan(int fd, const t_strvect &split, IRCserv *serv, t_citer client_it)
{
	std::list<Channel>::iterator	chan;
	std::vector<std::string>		args;
	std::vector<std::string>		keys;
	size_t	i;

	args = ft_splitstring(split[1], ",");
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
				if (chan->isOnChan(&(*client_it)))
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
					if (!client_it->isOperator())
					{
						serv->fds[fd].wrbuf += ":" + serv->servername + " 481 " + args[i] +
							":Permission Denied- You're not an IRC operator" + CRLF;
						break ;
					}
				if (chan->getflags()._invite_only)
					if (!client_it->isInvited(&(*chan)))
					{
						serv->fds[fd].wrbuf += ":" + serv->servername + " 473 " +
							args[i] + " :Cannot join channel (+i)" + CRLF;
						break ;
					}
				if (chan->getflags()._ban_mask.size() > 0)
					if (chan->isBanned(client_it->getptr()))
					{
						serv->fds[fd].wrbuf += ":" + serv->servername + " 474 " + args[i] +
							+ " :Cannot join channel (+b)" + CRLF;
						break ;
					}
				chan->add_client(client_it->getptr());
				client_it->add_channel(chan->getptr());
				if (args[i][0] != '&')
					msg_forward(-1, ":" + client_it->getnickname() + " JOIN " + args[i], serv);
				join_backward(serv, chan, client_it);
				break ;
			}
		if (chan == serv->channels.end())
		{
			serv->channels.push_back(Channel(args[i], keys[i], client_it->getptr()));
			client_it->add_channel((serv->channels.rbegin())->getptr());
			if (args[i][0] != '&')
				msg_forward(-1, ":" + client_it->getnickname() + " JOIN " + args[i], serv);
			if (!keys[i].empty())
				msg_forward(-1, "MODE " + args[i] + " +k " + keys[i], serv);
			join_backward(serv, --serv->channels.end(), client_it);
		}
	}
}

void	join_from_network(int fd, const t_strvect &split, IRCserv *serv)
{
	Client		*client;
	Channel		*channel;
	t_strvect	chans;
	t_strvect	modes;

	if (split.size() < 3)
		return ;
	if ((client = find_client_by_nick(get_nick_from_info(split[0]), serv)) == 0)
		return ;//bad command from server
	chans = ft_splitstring(split[2], ',');
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
		}
		else
		{
			channel->add_client(client->getptr());
			client->add_channel(channel->getptr());
		}
	}
	msg_forward(fd, strvect_to_string(split), serv);
}

void	cmd_join(int fd, t_strvect const &split, IRCserv *serv)
{
	t_citer	client_it;
	if (split.size() < 2)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, fd, "JOIN",
				"Not enough parameters");
		return ;
	}

	client_it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (split[0] == "JOIN" && (serv->fds[fd].type == FD_CLIENT ||
			serv->fds[fd].type == FD_OPER))
	{
		if (!client_it->isRegistred())
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1 , "JOIN",
					"You have not registered");
			return ;
		}
		join_to_chan(fd, split, serv, client_it);
	}
	else if (split[0][0] == ':' && serv->fds[fd].type == FD_SERVER)
		join_from_network(fd, split, serv);
}
