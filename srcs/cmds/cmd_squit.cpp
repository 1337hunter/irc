/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_squit.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:37:57 by salec             #+#    #+#             */
/*   Updated: 2020/11/04 16:38:08 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

void		cmd_squit(int fd, const t_strvect &split, IRCserv *_server)
{
	std::vector<server_server>::iterator	begin;
	std::vector<server_server>::iterator	end;

	while (begin != end && begin->host != split[1])
		begin++;
	if (begin != end)
	{
		_server->connect.erase(begin);
		FD_CLR(fd, &(_server->fdset_read));
		close(fd);
		_server->fds.erase(fd);
#if DEBUG_MODE
		std::cout << "server " << split[1] << ":\t\t\tdisconnected" << std::endl;
#endif
	}
}
