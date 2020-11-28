/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_oper.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbright <gbright@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/10 14:09:05 by gbright           #+#    #+#             */
/*   Updated: 2020/11/28 12:53:38 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"
#include "message.hpp"

void    cmd_oper(int fd, const t_strvect &split, IRCserv *serv)
{
	std::vector<t_oper>::iterator	b = serv->oper.begin();
	std::vector<t_oper>::iterator	e = serv->oper.end();
	t_citer	fd_entry;

	fd_entry = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (serv->fds[fd].type == FD_OPER)
	{
		serv->fds[fd].wrbuf += get_reply(serv, RPL_YOUREOPER, fd, "",
				"You are now an IRC operator");
		return ;
	}
	if (fd_entry == serv->clients.end() || !fd_entry->isRegistred())
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1, "",
				"You have not registered");
		return ;
	}
	if (split.size() < 3)
	{
		serv->fds[fd].wrbuf = ":" + serv->servername + " ";
		serv->fds[fd].wrbuf += ERR_NEEDMOREPARAMS;
		serv->fds[fd].wrbuf += " OPER :Not enough parameters";
		serv->fds[fd].wrbuf += CRLF;
		return ;
	}
	while (b != e)
	{
		if (b->name == split[1])
			break ;
		b++;
	}
	if (b == e)
	{
		serv->fds[fd].wrbuf = ":" + serv->servername + " ";
		serv->fds[fd].wrbuf += ERR_NOOPERHOST;
		serv->fds[fd].wrbuf += " :No O-lines for your host";
		serv->fds[fd].wrbuf += CRLF;
		return ;
	}
	if (b->pass != split[2])
	{
		serv->fds[fd].wrbuf = ":" + serv->servername + " ";
		serv->fds[fd].wrbuf += ERR_PASSWDMISMATCH;
		serv->fds[fd].wrbuf += " :Password incorrect";
		serv->fds[fd].wrbuf += CRLF;
		return ;
	}
	fd_entry->setOPER();
	serv->fds[fd].type = FD_OPER;
	serv->fds[fd].wrbuf += get_reply(serv, RPL_YOUREOPER, fd, "",
			"You are now an IRC operator");
	return ;
}
