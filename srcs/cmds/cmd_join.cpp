/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_join.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbright <gbright@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/01 15:42:46 by gbright           #+#    #+#             */
/*   Updated: 2020/12/04 15:36:18 by gbright          ###   ########.fr       */
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

void	join_forward(std::string &name, std::string &key, t_citer client_it, IRCserv *serv,
		std::string	append)
{
	std::vector<t_server>::iterator	net;

	for (net = serv->network.begin(); net != serv->network.end(); net++)
		serv->fds[net->fd].wrbuf += ":" + client_it->getnickname() + append +
		   " JOIN " + name + " " + key + CRLF;
}

void	join_backward(IRCserv *serv, std::list<Channel>::iterator chan, t_citer client_it)
{
	std::unordered_map<Client*, client_flags>::iterator c_map;
	std::string		reply;

	reply = ":" + client_it->getnickname() + "!" + client_it->getusername() +
		"@" + client_it->gethostname() + " JOIN :" + chan->getname() + CRLF;
	for (c_map = chan->getclients().begin(); c_map != chan->getclients().end(); c_map++)
		if (c_map->first->getFD() != client_it->getFD())
			serv->fds[c_map->first->getFD()].wrbuf += reply;
	serv->fds[client_it->getFD()].wrbuf += reply;
	serv->fds[client_it->getFD()].wrbuf += reply_chan_names(serv, chan, &(*client_it));
}

void	join_to_hash_chan(int fd, const t_strvect &split, IRCserv *serv, t_citer client_it)
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
				if (keys[i] == chan->getkey())
				{
					chan->add_client(&(*client_it));
					join_forward(args[i], keys[i], client_it, serv, "");
					join_backward(serv, chan, client_it);
				}
				else
					serv->fds[fd].wrbuf += get_reply(serv, ERR_BADCHANNELKEY, fd,
							args[i], "Cannot join channel (+k)");
				break ;
			}
		if (chan == serv->channels.end())
		{
			serv->channels.push_back(Channel(args[i], keys[i], &(*client_it)));
			join_forward(args[i], keys[i], client_it, serv, "");
			join_backward(serv, --serv->channels.end(), client_it);
		}
	}
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
		if (split[1][0] == '#')
			join_to_hash_chan(fd, split, serv, client_it);
	}
}
