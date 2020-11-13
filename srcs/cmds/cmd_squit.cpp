/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_squit.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:37:57 by salec             #+#    #+#             */
/*   Updated: 2020/11/06 23:07:05 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

void		cmd_squit(int fd, const t_strvect &split, IRCserv *serv)
{
	std::vector<t_server>::iterator	begin = serv->connect.begin();
	std::vector<t_server>::iterator	end = serv->connect.end();

	fd = 0;
	while (begin != end && begin->hostname != split[1])
		begin++;
	if (begin != end)
	{
		close(begin->fd);
		serv->fds.erase(begin->fd);
		FD_CLR(begin->fd, &(serv->fdset_read));
		FD_CLR(begin->fd, &(serv->fdset_write));
		serv->connect.erase(begin);

#if DEBUG_MODE
		std::cout << "server " << split[1] << ":\tdisconnected" << std::endl;
#endif
	}
}
