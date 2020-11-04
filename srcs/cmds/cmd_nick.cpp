/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_nick.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:29:56 by salec             #+#    #+#             */
/*   Updated: 2020/11/04 16:29:58 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

void		cmd_nick(int fd, const t_strvect &split, IRCserv *_server)
{
	std::string		reply;
	t_citer			nick_entry;
	t_citer			fd_entry;
	t_citer			end;

	end = _server->clients.end();
	fd_entry = ft_findclientfd(_server->clients.begin(), _server->clients.end(), fd);
	nick_entry = ft_findnick(_server->clients.begin(), _server->clients.end(), split[1]);
	if (nick_entry == end && fd_entry == end)
		_server->clients.push_back(Client(split[1], fd));
	else if (fd_entry != end)
	{
		// Here we also need to track nicknames
		// for KICK, MODE and KILL commands.
		reply = ":";
		reply += fd_entry->getnickname();
		fd_entry->ChangeNick(split[1]);
		reply += " NICK ";
		reply += fd_entry->getnickname();
		reply += CLRF;
		send(fd, reply.c_str(), reply.length(), 0);
	}
	else
	{
		reply = ":" + _server->hostname + " ";
		reply += ERR_NICKNAMEINUSE;
		reply += " " + split[1] + " :Nickname is already in use";
		reply += CLRF;
		send(fd, reply.c_str(), reply.length(), 0);
		/*	need to save the state in this case
		 *	cli will try to send another NICK after USER msg */
	}
}
