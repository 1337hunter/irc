/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircsock_tls.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/14 02:03:53 by salec             #+#    #+#             */
/*   Updated: 2021/01/20 17:11:07 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "message.hpp"
#include "tools.hpp"

void	InitSSLCTX(IRCserv *serv)
{
	/* check if cert exists */
	int	fd = open("./conf/ircserv.crt", O_RDONLY);
	if (fd < 0)
	{
		serv->sslctx = NULL;
		std::cerr << "Certificate not found. Skipping SSL_CTX creation" <<
			std::endl;
		return ;
	}
	close(fd);

	/* ERR_free_strings() may be needed if we want to cleanup memory */
	/* SSL_connect won't work with TLS_server_method	*/
	if (!(serv->sslctx = SSL_CTX_new(TLS_method())))
	{
		ERR_print_errors_cb(SSLErrorCallback, NULL);
		error_exit("Unable to create SSL context");
	}
	if (SSL_CTX_set_ecdh_auto(ctx, 1) <= 0)
	{
		ERR_print_errors_cb(SSLErrorCallback, NULL);
		error_exit("SSL_CTX_set_ecdh_auto failed");
	}

	/* Set the key and cert */
	if (SSL_CTX_use_certificate_file(serv->sslctx,
		"./conf/ircserv.crt", SSL_FILETYPE_PEM) <= 0)
	{
		ERR_print_errors_cb(SSLErrorCallback, NULL);
		error_exit("Failed to load a certificate");
	}
	if (SSL_CTX_use_PrivateKey_file(serv->sslctx,
		"./conf/ircserv.key", SSL_FILETYPE_PEM) <= 0)
	{
		ERR_print_errors_cb(SSLErrorCallback, NULL);
		error_exit("Failed to load a private key");
	}
}

void	CreateSockTLS(IRCserv *serv, t_listen &_listen)
{
	t_sockaddr_in	sockin;
	t_protoent		*pe = NULL;
	int				optval = 1;

	if (serv->sslctx == NULL)
	{
		std::cerr << "Skipping TLS server creation. No context." << std::endl;
		return ;
	}
	if (!(pe = getprotobyname("tcp")))
		error_exit("getprotobyname error");
	if ((_listen.socket_fd = socket(PF_INET, SOCK_STREAM, pe->p_proto)) < 0)
		error_exit("socket error");
	if (fcntl(_listen.socket_fd, F_SETFL, O_NONBLOCK) < 0)
		error_exit("fcntl error: failed to set nonblock fd");
	sockin.sin_family = AF_INET;
	if (_listen.ip == "*")
		sockin.sin_addr.s_addr = INADDR_ANY;
	else
		sockin.sin_addr.s_addr = inet_addr(_listen.ip.c_str());
	sockin.sin_port = htons(_listen.port);
	if (setsockopt(_listen.socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
		error_exit("set socket option returned error");
	if (bind(_listen.socket_fd, (t_sockaddr*)&sockin, sizeof(sockin)) < 0)
		error_exit("bind error (probably already binded)");
	if (listen(_listen.socket_fd, 42) < 0)
		error_exit("listen error");

	t_fd	&fdref = serv->fds[_listen.socket_fd];		// this will create t_fd and return ref
	fdref.type = FD_ME;
	fdref.tls = true;
	fdref.status = true;
	fdref.fatal = false;
	fdref.inprogress = false;
	fdref.dtopened = ft_getcurrenttime();
	fdref.sentmsgs = 0;
	fdref.recvmsgs = 0;
	fdref.sentbytes = 0;
	fdref.recvbytes = 0;
	fdref.sock = -1;
	fdref.linkname = serv->servername + "[" + inet_ntoa(sockin.sin_addr) +
		":" + TOSTRING(_listen.port) + "]";
#if DEBUG_MODE
	std::cout << "tlsserver created on socket " << _listen.socket_fd <<
		" (port " << _listen.port << ")" << std::endl;
#endif
}

void	DoHandshakeTLS(int fd, IRCserv *serv)
{
	t_fd	&fdref = serv->fds[fd];
	int		handshake = 0;
	if (fdref.type == FD_SERVER)
		handshake = SSL_connect(fdref.sslptr);
	else
		handshake = SSL_accept(fdref.sslptr);

	// continue if handshake need more actions (until it returns 1)
	if (handshake != 1)
	{
		// check if handshake need more actions or gone wrong by SSL_get_error
		int	err = SSL_get_error(fdref.sslptr, handshake);
		// SSL_ERROR_WANT_READ/WRITE in case handshake needs another round
		if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE)
		{
			// drop the connection if handshake gone wrong
			std::cerr << "TLS handshake failed for client " << fd << std::endl;
			std::string		sslerr;
			ERR_print_errors_cb(SSLErrorCallback, &sslerr);
			if (fdref.type == FD_SERVER && sslerr.length() == 0)
			{
				sslerr = "TLS handshake failed for server ";
				sslerr += fd;
			}
			if (fdref.type == FD_SERVER)
				msg_error("SSL_connect: " + sslerr, serv);
			// we shouldn't call SSL_shutdown because it's already fatal
			SSL_free(serv->fds[fd].sslptr);
			close(fd);
			serv->fds.erase(fd);
		}
	}
}
