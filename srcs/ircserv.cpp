/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 16:44:15 by salec             #+#    #+#             */
/*   Updated: 2020/11/17 20:03:55 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

std::string const	IRCserv::clrf = CRLF;

void	do_socket(IRCserv *serv)
{
	std::vector<t_listen>::iterator lb = serv->listen.begin();
	std::vector<t_listen>::iterator le = serv->listen.end();
	t_listen	_listen;

	 if (lb == le)
		 error_exit("Config error. Block listen didn't specified");
	 while (lb != le)
	 {
		if (lb->tls == false)
			CreateSock(serv, *lb);
		else
			CreateSockTLS(serv, *lb);
		lb++;
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
				std::cout << serv->fds[fd].sslptr << "\n\n" << std::endl;
				if (!(serv->fds[fd].tls) || (serv->fds[fd].tls &&
					SSL_is_init_finished(serv->fds[fd].sslptr)))
				{
					SendMessage(fd, serv);
				}
			}
		}
	}
}
