/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_error.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbright <gbright@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/13 13:15:50 by gbright           #+#    #+#             */
/*   Updated: 2020/11/15 10:45:39 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"

void	cmd_error(int fd, const t_strvect &split, IRCserv *serv)
{
	size_t	i;
	t_citer	b = serv->clients.begin();
	t_citer e = serv->clients.end();
	std::vector<t_server>::iterator sb = serv->connect.begin();
	std::vector<t_server>::iterator se = serv->connect.end();

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
			if (b->isOperator())
			{
				serv->fds[b->getFD()].wrbuf += "NOTICE " + b->getnickname() +
					" :ERROR from " + sb->hostname + " -- ";
				i = 1;
				if (split.size() > 1)
				{
					std::string	temp(split[1], 1);
					serv->fds[b->getFD()].wrbuf += temp;
				}
				while (++i < split.size())
					serv->fds[b->getFD()].wrbuf += split[i];
			}
			b++;
		}
	}
	else
		msg_error("Internal error. Server not found while ERROR processes", serv);
}
