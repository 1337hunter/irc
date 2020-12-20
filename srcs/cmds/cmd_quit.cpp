/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_quit.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:35:26 by salec             #+#    #+#             */
/*   Updated: 2020/12/20 14:09:32 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"

int         quit_from_network(int fd, t_strvect const &split, IRCserv *serv)
{
	fd = 0; (void)split; serv = 0;
	return 0;
}


int			quit_from_client(int fd, t_strvect const &split, IRCserv *serv)
{
	t_citer 	it;
	std::string	quit_msg;

	if (split.size() > 1)
		quit_msg = strvect_to_string(split, ' ', 1);
	else
		quit_msg = ":Default";
	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (it == serv->clients.end() || !it->isRegistred())
	{
		serv->fds[fd].wrbuf += "ERROR :Closing Link: [" + serv->fds[fd].hostname + "]\r\n";
		serv->fds[fd].status = false; return 1;
	}
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
	addtonickhistory(serv, it);
	serv->clients.erase(it);
#if DEBUG_MODE
	std::cout << "client " << fd << "\t\tdisconnected" << std::endl;
#endif
	return 0;
}

void		cmd_quit(int fd, const t_strvect &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_SERVER)
		quit_from_network(fd, split, serv);
	else
		quit_from_client(fd, split, serv);
}
