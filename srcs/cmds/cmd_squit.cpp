/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_squit.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:37:57 by salec             #+#    #+#             */
/*   Updated: 2020/11/06 19:53:56 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

void		cmd_squit(int fd, const t_strvect &split, IRCserv *_server)
{
	std::vector<t_server>::iterator	begin;
	std::vector<t_server>::iterator	end;

	while (begin != end && begin->hostname != split[1])
		begin++;
	if (begin != end)
	{
		_server->connect.erase(begin);
#if DEBUG_MODE
		std::cout << "server " << split[1] << ":\t\t\tdisconnected" << std::endl;
#endif
	}
	FD_CLR(fd, &(_server->fdset_read));
	FD_CLR(fd, &(_server->fdset_write));
	close(fd);
	_server->fds.erase(fd);
}
