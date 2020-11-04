/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_pass.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:35:58 by salec             #+#    #+#             */
/*   Updated: 2020/11/04 17:25:50 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

void		cmd_pass(int fd, const t_strvect &split, IRCserv *_server)
{
	if (split.size() < 2)
	{
		std::string	reply;
		reply = ":" + _server->hostname + " ";
		reply += ERR_NEEDMOREPARAMS;
		reply += " :not enough parameters";
		reply += CLRF;
//		send(fd, reply.c_str(), reply.length(), 0);
		_server->fds[fd].wrbuf += reply;
		return ;
	}
	_server->fds[fd].pass = split[1];
	if (split.size() == 2)
		return ;
	_server->fds[fd].version = split[2];
	if (split.size() == 3)
		return ;
	_server->fds[fd].flags = split[3];
	if (split.size() == 4)
		return ;
	_server->fds[fd].options = split[4];
}
