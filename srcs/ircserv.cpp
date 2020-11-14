/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 16:44:15 by salec             #+#    #+#             */
/*   Updated: 2020/11/14 05:41:08 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

std::string const	IRCserv::clrf = CRLF;

void	RunServer(IRCserv *serv)
{
	CreateSock(serv);
	CreateSockTLS(serv);
	while (1)
	{
		int	lastfd = 0;
		FD_ZERO(&(serv->fdset_read));
		FD_ZERO(&(serv->fdset_write));
		FD_ZERO(&(serv->fdset_error));
		for (std::map<int, t_fd>::iterator it = serv->fds.begin();
			it != serv->fds.end(); it++)
		{
			FD_SET(it->first, &(serv->fdset_read));
			if (!(it->second.wrbuf.empty()))
				FD_SET(it->first, &(serv->fdset_write));
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
			if (isread)
			{
				if (serv->fds[fd].type == FD_ME)
					AcceptConnect(serv, serv->fds[fd].tls);
				else
				{
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
			}
		}
	}
}
