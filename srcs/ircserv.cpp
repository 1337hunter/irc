/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 16:44:15 by salec             #+#    #+#             */
/*   Updated: 2020/12/19 21:15:41 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

void	do_socket(IRCserv *serv)
{
	std::vector<t_listen>::iterator lb;
	t_listen	_listen;

	for (lb = serv->listen.begin(); lb != serv->listen.end(); lb++)
	{
		if (lb->tls == false)
			CreateSock(serv, *lb);
		else
			CreateSockTLS(serv, *lb);
	}
	_listen.ip = "*";
	_listen.port = serv->port;
	_listen.tls = false;
	_listen.ssl = false;
	_listen.serveronly = false;
	CreateSock(serv, _listen);
}

void	RunServer(IRCserv *serv)
{
	do_socket(serv);
	while (1)
	{
		int	lastfd = 0;
		FD_ZERO(&(serv->fdset_read));
		FD_ZERO(&(serv->fdset_write));
		FD_ZERO(&(serv->fdset_error));
		clear_kill_list(serv);
		for (std::map<int, t_fd>::iterator it = serv->fds.begin();
			it != serv->fds.end(); it++)
		{
			FD_SET(it->first, &(serv->fdset_read));
			if (!(it->second.wrbuf.empty()))
				FD_SET(it->first, &(serv->fdset_write));
			FD_SET(it->first, &(serv->fdset_error));
			lastfd = std::max(lastfd, it->first);
		}
		int readyfds = select(lastfd + 1,
			&(serv->fdset_read), &(serv->fdset_write),
			&(serv->fdset_error), NULL);
		if (readyfds < 0)
			error_exit(SELECT);
		for (int fd = 0; readyfds > 0 && fd <= lastfd; fd++)
		{
			bool	isread = FD_ISSET(fd, &(serv->fdset_read));
			bool	iswrite = FD_ISSET(fd, &(serv->fdset_write));
			bool	iserror = FD_ISSET(fd, &(serv->fdset_error));

			if (isread || iswrite || iserror)
				readyfds--;
			if (iserror)
			{
				std::cerr << "some error happened on fd " << fd << std::endl;
				continue ;
			}
			if ((isread || iswrite) && didSockFail(fd, serv))
				continue ;
			if (isread)
			{
				if (serv->fds[fd].type == FD_ME)
					AcceptConnect(fd, serv, serv->fds[fd].tls);
				else
				{
					// only allow non tls and handshaked tls data exchange
					if (!(serv->fds[fd].tls) || (serv->fds[fd].tls &&
						SSL_is_init_finished(serv->fds[fd].sslptr)))
						ReceiveMessage(fd, serv);
					else
						DoHandshakeTLS(fd, serv);
				}
			}
			if (iswrite)
			{
				if (!(serv->fds[fd].tls) || (serv->fds[fd].tls &&
					SSL_is_init_finished(serv->fds[fd].sslptr)))
					SendMessage(fd, serv);
				else
					DoHandshakeTLS(fd, serv);
			}
		}
	}
}
