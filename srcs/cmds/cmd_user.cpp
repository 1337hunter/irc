/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_user.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:30:28 by salec             #+#    #+#             */
/*   Updated: 2020/11/04 17:23:25 by salec            ###   ########.fr       */
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
		reply = ":" + _server->hostname + " ";
		reply += ERR_NEEDMOREPARAMS;
		reply += " " + split[0];
		reply += " :Not enough parameters";
		reply += CLRF;
	}
	else
	{
		it = ft_findclientfd(_server->clients.begin(), _server->clients.end(), fd);
		if (it != _server->clients.end())
		{
			if (it->Register(split[1], split[4]))
			{
				reply = ":" + _server->hostname + " ";
				reply += RPL_WELCOME;
				reply += " " + it->getnickname() +
					" :Welcome to the Internet Relay Network " +
					it->getnickname() + "!" +
					it->getnickname() + "@" + _server->hostname + CLRF;
			}
			else
			{
				reply = ":" + _server->hostname + " ";
				reply += ERR_ALREADYREGISTRED;
				reply += " :Unauthorized command (already registered)";
				reply += CLRF;
			}
		}
	}
	_server->fds[fd].wrbuf += reply;
//	send(fd, reply.c_str(), reply.length(), 0);
}
