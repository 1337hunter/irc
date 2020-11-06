/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_user.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:30:28 by salec             #+#    #+#             */
/*   Updated: 2020/11/06 19:55:24 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

void		cmd_user(int fd, const t_strvect &split, IRCserv *_server)
{
	std::string		reply;
	t_citer			it;

	if (split.size() < 5)
	{
		reply = ft_buildmsg(_server->hostname, ERR_NEEDMOREPARAMS, "*", split[0],
			"Not enough parameters");
	}
	else
	{
		it = ft_findclientfd(_server->clients.begin(), _server->clients.end(), fd);
		if (it != _server->clients.end())
		{
			if (it->Register(split[1], split[4]))
			{
				reply += ft_buildmsg(_server->hostname, RPL_WELCOME,
					it->getnickname(), "",
					"Welcome to the Internet Relay Network " +
					it->getnickname() + "!" + it->getnickname() + "@" +
					_server->hostname);
				reply += ft_buildmsg(_server->hostname, RPL_YOURHOST,
					it->getnickname(), "", "Your host is " + _server->hostname +
					", running version " + /*_server->version*/ "0.1");
				reply += ft_buildmsg(_server->hostname, RPL_CREATED,
					it->getnickname(), "", "This server was created <date>");
				reply += ft_buildmsg(_server->hostname, RPL_MYINFO,
					it->getnickname(), "", _server->hostname + " " +
					/*_server->version*/ "0.1" + " " +
					"<available user modes>" + " " +
					"<available channel modes>");
			}
			else
				reply = ft_buildmsg(_server->hostname, ERR_ALREADYREGISTRED,
					it->getnickname(), "", ":You may not reregister");
		}
	}
	_server->fds[fd].wrbuf += reply;
}
