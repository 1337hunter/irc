/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_error.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/13 13:15:50 by gbright           #+#    #+#             */
/*   Updated: 2021/01/21 17:36:00 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

void	cmd_error(int fd, const t_strvect &split, IRCserv *serv)
{
	t_citer	b = serv->clients.begin();
	t_citer e = serv->clients.end();
	std::list<t_server>::iterator sb = serv->network.begin();
	std::list<t_server>::iterator se = serv->network.end();

	while (sb != se)
	{
		if (sb->fd == fd)
			break ;
		sb++;
	}
	if (sb != se)
	{
		while (b != e)
		{
			if  (b->isOperator() && !b->isAway())
			{
				serv->fds[b->getFD()].wrbuf += "NOTICE " + b->getnick() +
					" :ERROR from " + sb->servername + " -- ";
				if (split.size() > 1)
				{
					std::string	temp(split[1], 1);
					serv->fds[b->getFD()].wrbuf += temp;
					serv->fds[b->getFD()].wrbuf += " ";
				}
				serv->fds[b->getFD()].wrbuf += strvect_to_string(split, ' ', 2);
				serv->fds[b->getFD()].wrbuf += CRLF;
			}
			b++;
		}
	}
	else
		msg_error("Internal error. Server not found while ERROR processes", serv);
}
