/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_join.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbright <gbright@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/01 15:42:46 by gbright           #+#    #+#             */
/*   Updated: 2020/12/01 23:03:59 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//Command: JOIN
//Parameters: <channel>[ %x7 <modes> ] *( "," <channel>[ %x7 <modes> ] )

#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"

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
					chan->add_client(&(*client_it));
				else
					serv->fds[fd].wrbuf += get_reply(serv, ERR_BADCHANNELKEY, fd,
							args[i], "Cannot join channel (+k)");
				break ;
			}
		if (chan == serv->channels.end())
		{
			//create new chan
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
	if (split[0] == "JOIN" && serv->fds[fd].type == FD_CLIENT)
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
	/*
	t_citer client_it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	serv->fds[fd].wrbuf += ":" + client_it->getnickname() + "!" +
		client_it->getusername() + "@" + client_it->gethostname() + " " + "JOIN " +
		":" + split[1] + CRLF;
	serv->fds[fd].wrbuf += ":" + serv->servername + " " + "MODE " + split[1] +
		" " + "+nt" + CRLF + ":" + serv->servername + " " + "353" + " " +
		client_it->getnickname() +  " " + "=" " " + split[1] + " " + ":" + "@" +
		client_it->getnickname() + " nick1 " + "nick2 " + CRLF;
		*/
}
