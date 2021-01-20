#include "ircserv.hpp"
#define TIMEOUTSEC 10

extern bool	g_server_die;
extern bool	g_rehash;

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

void	ircserv_free(IRCserv *serv)
{
	std::MAP<int, t_fd>::iterator	fd;

	fd = serv->fds.begin();
	for (; fd != serv->fds.end(); fd++)
	{
		if (fd->second.tls)
		{
			SSL_shutdown(fd->second.sslptr);
			SSL_free(fd->second.sslptr);
		}
		close(fd->first);
	}
	SSL_CTX_free(serv->sslctx);
	delete (serv);
}

void	RunServer(IRCserv *serv)
{
	bool	die = false;
	bool	rehash = false;

	do_socket(serv);
	while (1)
	{
		int		lastfd = 0;
		timeval	timeout = {TIMEOUTSEC, 0};	// timeout needed only in this scope
		if (g_server_die)
			die = true;
		if (g_rehash)
			rehash = true;
		FD_ZERO(&(serv->fdset_read));
		FD_ZERO(&(serv->fdset_write));
		FD_ZERO(&(serv->fdset_error));
		clear_kill_list(serv);
		clear_block_list(serv);
		clear_empty_channels(serv);
		clear_whowas(serv);
		check_liveness(serv);
		for (std::MAP<int, t_fd>::iterator it = serv->fds.begin();
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
		if (readyfds < 0)
			error_exit(SELECT);
		for (int fd = 0; readyfds > 0 && fd <= lastfd; fd++)
		{
			errno = 0;	// better reset errno each fd
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
			if (FD_ISSET(fd, &(serv->fdset_read)))	// double check
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
			if (FD_ISSET(fd, &(serv->fdset_write)))	// double check
			{
				if (!(serv->fds[fd].tls) || (serv->fds[fd].tls &&
					SSL_is_init_finished(serv->fds[fd].sslptr)))
					SendMessage(fd, serv);
				else
					DoHandshakeTLS(fd, serv);
			}
		}
		if (die)
			exit(0);
		if (rehash)
		{
			ircserv_free(serv);
			return ;
		}
	}
}
