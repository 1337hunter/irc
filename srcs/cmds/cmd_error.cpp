/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_error.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbright <gbright@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/13 13:15:50 by gbright           #+#    #+#             */
/*   Updated: 2020/11/13 15:03:12 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"

void	cmd_error(int fd, const t_strvect &split, IRCserv *serv)
{
	t_citer fd_entry;
	std::vector<t_server>::iterator sb;
	std::vector<t_server>::iterator se = serv->connect.end();
	std::map<int, t_fd>::iterator	mb = serv->fds.begin();
	std::map<int, t_fd>::iterator	me = serv->fds.end();
	size_t	i;

	while (mb != me)
	{
		if (mb->second.type == FD_OPER)
		{
			fd_entry = ft_findclientfd(serv->clients.begin(), serv->clients.end(),
					mb->first);
			if (fd_entry != serv->clients.end())
			{
				sb = serv->connect.begin();
				while (sb != se)
				{
					if (sb->fd == fd)
						break ;
					sb++;
				}
				if (sb != se)
				{
					serv->fds[mb->first].wrbuf += "NOTICE " + fd_entry->getnickname() +
						" :ERROR from " + sb->hostname + " -- ";
					i = 1;
					if (split.size() > 1)
					{
						std::string	temp(split[1], 1);
						serv->fds[mb->first].wrbuf += temp;
					}
					while (++i < split.size())
						serv->fds[mb->first].wrbuf += split[i];
				}
			}
		}
		mb++;
	}
}
