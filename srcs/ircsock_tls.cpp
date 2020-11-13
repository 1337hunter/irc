/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircsock_tls.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/14 02:03:53 by salec             #+#    #+#             */
/*   Updated: 2020/11/14 02:26:09 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

int		InitSSLCTX(IRCserv *serv)
{
	/* check if cert exists */
	int	fd = open("./conf/ircserv.crt", O_RDONLY);
	if (fd < 0)
	{
		std::cerr << "Certificate not found. Skipping TLS server creation" <<
			std::endl;
		return (1);
	}
	close(fd);

	/* init ssl lib */
	SSL_library_init();
	if (!(serv->sslctx = SSL_CTX_new(TLS_server_method())))
		error_exit("Unable to create SSL context");
	if (SSL_CTX_set_ecdh_auto(ctx, 1) <= 0)
		error_exit("SSL_CTX_set_ecdh_auto failed");

	/* Set the key and cert */
	if (SSL_CTX_use_certificate_file(serv->sslctx,
		"./conf/ircserv.crt", SSL_FILETYPE_PEM) <= 0)
		error_exit("Failed to load a certificate");
	if (SSL_CTX_use_PrivateKey_file(serv->sslctx,
		"./conf/ircserv.key", SSL_FILETYPE_PEM) <= 0)
		error_exit("Failed to load a private key");
	return (0);
}

void	CreateSockTLS(IRCserv *serv)
{
	t_sockaddr_in	sockin;
	t_protoent		*pe = NULL;
	int				optval = 1;

	if (InitSSLCTX(serv))
		return ;
	serv->tls_port = serv->port + 1;	// temp tls port = basic port + 1
	if (!(pe = getprotobyname("tcp")))
		error_exit("getprotobyname error");
	if ((serv->tls_sock = socket(PF_INET, SOCK_STREAM, pe->p_proto)) < 0)
		error_exit("socket error");
	if (fcntl(serv->tls_sock, F_SETFL, O_NONBLOCK) < 0)
		error_exit("fcntl error: failed to set nonblock fd");
	sockin.sin_family = AF_INET;
	sockin.sin_addr.s_addr = /*INADDR_ANY;*/ inet_addr("127.0.0.1");
	//	can change ip to create another local server without the b8s or
	//	containers just with ip variable seted in config file
	sockin.sin_port = htons(serv->tls_port);

	if (setsockopt(serv->tls_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
		error_exit("set socket option returned error");
	if (bind(serv->tls_sock, (t_sockaddr*)&sockin, sizeof(sockin)) < 0)
		error_exit("bind error (probably already binded)");
	if (listen(serv->tls_sock, 42) < 0)
		error_exit("listen error");

	serv->fds[serv->tls_sock].type = FD_ME;
	serv->fds[serv->tls_sock].tls = true;
	std::cout << "server(tls) created on socket " << serv->tls_sock << std::endl;
}

void	DoHandshakeTLS(int fd, IRCserv *serv)
{
	int		handshakeres = SSL_accept(serv->fds[fd].sslptr);

	// continue if handshake need more actions (until it returns 1)
	if (handshakeres == 1 && SSL_is_init_finished(serv->fds[fd].sslptr))
		serv->fds[fd].handshaked = true;
	else
	{
		// check if handshake need more actions or gone wrong by SSL_get_error
		int	err = SSL_get_error(serv->fds[fd].sslptr, handshakeres);
		// SSL_ERROR_WANT_READ/WRITE in case handshake needs another round
		if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE)
		{
			// drop the connection if handshake gone wrong
			std::cerr << "TLS handshake failed for client " << fd << std::endl;
			SSL_shutdown(serv->fds[fd].sslptr);
			SSL_free(serv->fds[fd].sslptr);
			close(fd);
			serv->fds.erase(fd);
		}
	}
}

void	AcceptConnectTLS(IRCserv *serv)
{
	int				fd;
	t_sockaddr_in	csin;
	socklen_t		csin_len;

	csin_len = sizeof(csin);
	if ((fd = accept(serv->tls_sock, (t_sockaddr*)&csin, &csin_len)) < 0)
		error_exit("accept error");
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		error_exit("fcntl error: failed to set nonblock fd");
	std::cout << "client(tls) " << fd << " accepted from\t" <<
		inet_ntoa(csin.sin_addr) << ":" << ntohs(csin.sin_port) << std::endl;
	serv->fds[fd].type = FD_CLIENT;
	// we dont know either it's client or other server
	serv->fds[fd].rdbuf.erase();
	serv->fds[fd].wrbuf.erase();
	serv->fds[fd].status = true;
	serv->fds[fd].tls = true;
	serv->fds[fd].handshaked = false;

	if (!(serv->fds[fd].sslptr = SSL_new(serv->sslctx)))
		error_exit("SSL_new failed");
	SSL_set_fd(serv->fds[fd].sslptr, fd);

}

void	ReceiveMessageTLS(int fd, IRCserv *serv)
{
	ssize_t		r;
	char		buf_read[BUF_SIZE + 1];

	if ((r = SSL_read(serv->fds[fd].sslptr, buf_read, BUF_SIZE)) >= 0)
		buf_read[r] = 0;
	if (r > 0)
	{
		serv->fds[fd].rdbuf += buf_read;
		if (serv->fds[fd].rdbuf.rfind(CRLF) + serv->clrf.length() ==
			serv->fds[fd].rdbuf.length())
		{
#if DEBUG_MODE
			std::cout << "client(tls) " << fd << " sent:\t\t" << serv->fds[fd].rdbuf;
#endif
			t_strvect	split = ft_splitstring(serv->fds[fd].rdbuf, CRLF);
			for (size_t i = 0; i < split.size(); i++)
				ProcessMessage(fd, split[i], serv);
			try { serv->fds.at(fd).rdbuf.erase(); }
			catch (std::out_of_range const &e) { (void)e; }
		}
	}
	else
	{
		close(fd);
		SSL_shutdown(serv->fds[fd].sslptr);
		SSL_free(serv->fds[fd].sslptr);
		serv->fds.erase(fd);
		t_citer	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
		if (it != serv->clients.end())
			it->Disconnect();
		std::cout << "client(tls) " << fd << "\t\tdisconnected" << std::endl;
	}
}

void	SendMessageTLS(int fd, IRCserv *serv)
{
	std::string	reply;

	if (serv->fds[fd].wrbuf.length() > BUF_SIZE)
	{
		reply = serv->fds[fd].wrbuf.substr(0, BUF_SIZE);
		serv->fds[fd].wrbuf = serv->fds[fd].wrbuf.substr(BUF_SIZE);
	}
	else
	{
		reply = serv->fds[fd].wrbuf;
		serv->fds[fd].wrbuf.erase();
	}
#if DEBUG_MODE
	std::cout << "sending client(tls) " << fd << "\t" << reply;
#endif
	ssize_t	r = SSL_write(serv->fds[fd].sslptr, reply.c_str(), reply.length());
	if (r <= 0 || serv->fds[fd].status == false)
	{
		close(fd);
		SSL_shutdown(serv->fds[fd].sslptr);
		SSL_free(serv->fds[fd].sslptr);
		serv->fds.erase(fd);
		t_citer	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
		if (it != serv->clients.end())
			it->Disconnect();
		std::cout << "client(tls) " << fd << ":\t\tdisconnected" << std::endl;
	}
}