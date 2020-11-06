/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_user.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:30:28 by salec             #+#    #+#             */
/*   Updated: 2020/11/04 20:22:20 by salec            ###   ########.fr       */
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
				reply += ":" + _server->hostname + " ";
				reply += RPL_YOURHOST;
				reply += " " + it->getnickname() +
					" :Your host is " + _server->hostname +
					", running version " + /*_server->version*/ "0.1" + CLRF;
				reply += ":" + _server->hostname + " ";
				reply += RPL_CREATED;
				reply += " " + it->getnickname() +
					" :This server was created " + "<date>" + CLRF;
				reply += ":" + _server->hostname + " ";
				reply += RPL_MYINFO;
				reply += " " + it->getnickname() + " :" +
					_server->hostname + " " +
					/*_server->version*/ "0.1" + " " +
					"<available user modes>" + " " +
					"<available channel modes>" + CLRF;
			}
			else
			{
				reply = ":" + _server->hostname + " ";
				reply += ERR_ALREADYREGISTRED;
				reply += " " + it->getnickname();
				reply += " :Unauthorized command (already registered)";
				reply += CLRF;
			}
		}
	}
	_server->fds[fd].wrbuf += reply;
//	send(fd, reply.c_str(), reply.length(), 0);
}
