/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_nick.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:29:56 by salec             #+#    #+#             */
/*   Updated: 2020/11/04 18:22:25 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

void		cmd_nick(int fd, const t_strvect &split, IRCserv *_server)
{
	std::string		reply;
	t_citer			nick_entry;
	t_citer			fd_entry;

	fd_entry = ft_findclientfd(_server->clients.begin(), _server->clients.end(), fd);
	nick_entry = ft_findnick(_server->clients.begin(), _server->clients.end(), split[1]);
	if (nick_entry == _server->clients.end() && fd_entry == _server->clients.end())
		_server->clients.push_back(Client(split[1], fd));
	else if (nick_entry != _server->clients.end() && !(fd_entry->isConnected()))
	{
		nick_entry->Reconnect(fd);
	}
	else if (fd_entry != _server->clients.end())
	{
		// Here we also need to track nicknames
		// for KICK, MODE and KILL commands.
		reply = ":";
		reply += fd_entry->getnickname();
		fd_entry->ChangeNick(split[1]);
		reply += " NICK ";
		reply += fd_entry->getnickname();
		reply += CLRF;
	}
	else
	{
		reply = ":" + _server->hostname + " ";
		reply += ERR_NICKNAMEINUSE;
		reply += " " + split[1] + " :Nickname is already in use";
		reply += CLRF;
		/*	need to save the state in this case
		 *	cli will try to send another NICK after USER msg */
	}
	_server->fds[fd].wrbuf += reply;
//	send(fd, reply.c_str(), reply.length(), 0);
}
