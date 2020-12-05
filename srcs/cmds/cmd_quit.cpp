/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_quit.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:35:26 by salec             #+#    #+#             */
/*   Updated: 2020/12/05 06:20:46 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"

#if 0
void		cmd_quit(int fd, IRCserv *serv)
{
	FD_CLR(fd, &(serv->fdset_read));
	FD_CLR(fd, &(serv->fdset_write));
	close(fd);
	serv->fds.erase(fd);
	t_citer it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (it != serv->clients.end())
	{
		addtonickhistory(serv, it);
		serv->clients.erase(it);
	}
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
	if (serv->fds[fd].tls)
	{
		SSL_shutdown(serv->fds[fd].sslptr);
		SSL_free(serv->fds[fd].sslptr);
	}
	serv->fds.erase(fd);
	t_citer it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (it != serv->clients.end())
	{
		addtonickhistory(serv, it);
		serv->clients.erase(it);
	}
#if DEBUG_MODE
	std::cout << "client " << fd << "\t\tdisconnected" << std::endl;
#endif
}
