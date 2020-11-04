/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_quit.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:35:26 by salec             #+#    #+#             */
/*   Updated: 2020/11/04 16:35:29 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

void		cmd_quit(int fd, const t_strvect &split, IRCserv *_server)
{
	(void)split;
	FD_CLR(fd, &(_server->fdset_read));
	close(fd);
	_server->fds.erase(fd);
	t_citer it = ft_findclientfd(_server->clients.begin(), _server->clients.end(), fd);
	if (it != _server->clients.end())
		it->Disconnect();
#if DEBUG_MODE
	std::cout << "client " << fd << ":\t\t\tdisconnected" << std::endl;
#endif
}
