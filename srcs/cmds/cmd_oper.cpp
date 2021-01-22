/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_oper.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/10 14:09:05 by gbright           #+#    #+#             */
/*   Updated: 2021/01/22 14:34:47 by gbright          ###   ########.fr       */
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
	if (serv->fds[fd].type == FD_OPER) {
		serv->fds[fd].wrbuf += get_reply(serv, RPL_YOUREOPER, fd, "",
				"You are now an IRC operator");
		return ;
	}
	if (fd_entry == serv->clients.end() || !fd_entry->isRegistred()) {
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1, "",
				"You have not registered");
		return ;
	}
	if (split.size() < 3) {
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, fd, "OPER",
				"Not enough parameters");
		return ;
	}
	while (b != e)
	{
		if (b->name == split[1])
			break ;
		b++;
	}
	if (b == e || !match(serv->fds[fd].hostname, b->hostmask)) {
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOOPERHOST, fd, "",
				"No O-lines for your host");
		return ;
	}
	if (b->pass != split[2]) {
		serv->fds[fd].wrbuf += get_reply(serv, ERR_PASSWDMISMATCH, fd, "",
				"Password incorrect");
		return ;
	}
	fd_entry->setOPER();
	fd_entry->setUMODE("+w");
	serv->fds[fd].type = FD_OPER;
	serv->fds[fd].wrbuf += get_reply(serv, RPL_YOUREOPER, fd, "",
			"You are now an IRC operator");
	msg_forward(fd, ":" + fd_entry->getnick() + " MODE " + fd_entry->getnick() + " +o", serv);
	serv->fds[fd].wrbuf += ":" + fd_entry->getinfo() + " MODE " + fd_entry->getnick() +
	" +o" + CRLF;
	return ;
}
