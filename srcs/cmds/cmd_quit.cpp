/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_quit.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:35:26 by salec             #+#    #+#             */
/*   Updated: 2020/11/11 22:24:19 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

#if 0
void		cmd_quit(int fd, IRCserv *_server)
{
	FD_CLR(fd, &(serv->fdset_read));
	FD_CLR(fd, &(serv->fdset_write));
	close(fd);
	serv->fds.erase(fd);
	t_citer it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (it != serv->clients.end())
		it->Disconnect();
#if DEBUG_MODE
    std::cout << "client " << fd << "\t\tdisconnected" << std::endl;
#endif
}
#endif

void		cmd_quit(int fd, const t_strvect &split, IRCserv *serv)
{
	(void)split;
	FD_CLR(fd, &(serv->fdset_read));
	FD_CLR(fd, &(serv->fdset_write));
	close(fd);
	serv->fds.erase(fd);
	t_citer it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (it != serv->clients.end())
		it->Disconnect();
#if DEBUG_MODE
	std::cout << "client " << fd << "\t\tdisconnected" << std::endl;
#endif
}
