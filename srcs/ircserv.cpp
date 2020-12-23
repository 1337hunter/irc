#include "ircserv.hpp"
#define TIMEOUTSEC 10

void	do_socket(IRCserv *serv)
{
	std::vector<t_listen>::iterator lb;
	t_listen	_listen;

	for (lb = serv->listen.begin(); lb != serv->listen.end(); lb++)
	{
		if (lb->tls == false && lb->ssl == false)
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
//	maybe insert here CreateSockTLS for default secure irc port 6697 (RFC 7194)
}

void	RunServer(IRCserv *serv)
{
	do_socket(serv);
	while (1)
	{
		int		lastfd = 0;
		timeval	timeout = {TIMEOUTSEC, 0};
		FD_ZERO(&(serv->fdset_read));
		FD_ZERO(&(serv->fdset_write));
		FD_ZERO(&(serv->fdset_error));
		clear_kill_list(serv);
		clear_block_list(serv);
		clear_empty_channels(serv);
		for (std::unordered_map<int, t_fd>::iterator it = serv->fds.begin();
			it != serv->fds.end(); it++)
		{
			if (!it->second.blocked)
			{
				FD_SET(it->first, &(serv->fdset_read));
				if (!(it->second.wrbuf.empty()))
					FD_SET(it->first, &(serv->fdset_write));
				FD_SET(it->first, &(serv->fdset_error));
				lastfd = std::max(lastfd, it->first);
			}
			else if (!it->second.wrbuf.empty())
				it->second.wrbuf.erase();
		}
		int readyfds = select(lastfd + 1,
			&(serv->fdset_read), &(serv->fdset_write),
			&(serv->fdset_error), &timeout);
//		if (DEBUG_MODE && readyfds == 0)
//			std::cout << "Server select tick (" << TIMEOUTSEC << "s)" << std::endl;
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
