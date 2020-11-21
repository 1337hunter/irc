/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_nick.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:29:56 by salec             #+#    #+#             */
/*   Updated: 2020/11/21 17:42:11 by salec            ###   ########.fr       */
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
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
			ERR_NONICKNAMEGIVEN, "", "NICK", "No nickname given");
		return ;
	}
	fd_entry = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	nick_entry = ft_findnick(serv->clients.begin(), serv->clients.end(), split[1]);
	if (nick_entry == serv->clients.end() && fd_entry == serv->clients.end())
		serv->clients.push_back(Client(split[1], fd));
	else if (nick_entry != serv->clients.end() && nick_entry->isConnected() &&
			!nick_entry->isRegistred() && fd_entry == serv->clients.end())
	{
		std::cout << "2\n";
		serv->fds[nick_entry->getFD()].status = false;
		serv->fds[nick_entry->getFD()].wrbuf = "Error :Closing Link: " + nick_entry->getnickname() + " (Overridden)";
		serv->fds[nick_entry->getFD()].wrbuf += CRLF;
		nick_entry->setFD(fd);
	}
	else if (nick_entry != serv->clients.end() && !(nick_entry->isConnected()))
	{
		std::cout << "3\n";
		nick_entry->Reconnect(fd);
	}
	else if (fd_entry != serv->clients.end() && fd_entry->isRegistred())
	{
		std::cout << "4\n";
		// Here we also need to track nicknames
		// for KICK, MODE and KILL commands.
		reply = ":";
		reply += fd_entry->getnickname();
		fd_entry->ChangeNick(split[1]);
		reply += " NICK ";
		reply += fd_entry->getnickname();
		reply += CRLF;
	}
	else if (fd_entry != serv->clients.end() && !fd_entry->isRegistred())
	{
		std::cout << "5\n";
		fd_entry->Register(split[1]);
		if (fd_entry->getUSER())
			reply = reply_welcome(serv, fd_entry->getnickname());
	}
	else
	{
		std::cout << "6\n";
		reply = ft_buildmsg(serv->servername, ERR_NICKNAMEINUSE, split[1], "",
			"Nickname is already in use");
		/*	need to save the state in this case
		 *	cli will try to send another NICK after USER msg */
	}
	serv->fds[fd].wrbuf += reply;
}
