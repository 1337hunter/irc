/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_ping.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:35:05 by salec             #+#    #+#             */
/*   Updated: 2020/11/04 16:35:06 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

void		cmd_ping(int fd, const t_strvect &split, IRCserv *_server)
{
	std::string		reply;

	if (split.size() < 2)
	{
		reply = ":" + _server->hostname + " ";
		reply += ERR_NOORIGIN;
		reply += " :No origin specified";
		reply += CLRF;
	}
	else
		reply = "PONG " + split[1] + CLRF;
	send(fd, reply.c_str(), reply.length(), 0);
}
