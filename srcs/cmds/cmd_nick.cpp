/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_nick.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:29:56 by salec             #+#    #+#             */
/*   Updated: 2020/11/10 22:13:39 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

void		cmd_nick(int fd, const t_strvect &split, IRCserv *serv)
{
	std::string		reply;
	t_citer			nick_entry;
	t_citer			fd_entry;

	if (split.size() < 2)
	{
		// may be different
		serv->fds[fd].wrbuf += ft_buildmsg(serv->hostname,
			ERR_NONICKNAMEGIVEN, "", "NICK", "No nickname given");
		return ;
	}
	fd_entry = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	nick_entry = ft_findnick(serv->clients.begin(), serv->clients.end(), split[1]);
	if (nick_entry == serv->clients.end() && fd_entry == serv->clients.end())
		serv->clients.push_back(Client(split[1], fd));
	else if (nick_entry != serv->clients.end() && !(fd_entry->isConnected()))
	{
		nick_entry->Reconnect(fd);
	}
	else if (fd_entry != serv->clients.end())
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
		reply = ft_buildmsg(serv->hostname, ERR_NICKNAMEINUSE, split[1], "",
			"Nickname is already in use");
		/*	need to save the state in this case
		 *	cli will try to send another NICK after USER msg */
	}
	serv->fds[fd].wrbuf += reply;
}
