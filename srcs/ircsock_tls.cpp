/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircsock_tls.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/14 02:03:53 by salec             #+#    #+#             */
/*   Updated: 2020/12/19 21:16:09 by salec            ###   ########.fr       */
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

	serv->fds[_listen.socket_fd].type = FD_ME;
	serv->fds[_listen.socket_fd].tls = true;
	serv->fds[_listen.socket_fd].status = true;
	serv->fds[_listen.socket_fd].dtopened = ft_getcurrenttime();
	serv->fds[_listen.socket_fd].sentmsgs = 0;
	serv->fds[_listen.socket_fd].recvmsgs = 0;
	serv->fds[_listen.socket_fd].sentbytes = 0;
	serv->fds[_listen.socket_fd].recvbytes = 0;
	serv->fds[_listen.socket_fd].sock = -1;
	serv->fds[_listen.socket_fd].linkname = serv->servername +
		"[" + inet_ntoa(sockin.sin_addr) + ":" + std::to_string(_listen.port) + "]";

	std::cout << "tlsserver created on socket " << _listen.socket_fd <<
		" (port " << _listen.port << ")" << std::endl;
}

void	DoHandshakeTLS(int fd, IRCserv *serv)
{
	int		handshake = 0;
	bool	isServer = (serv->fds[fd].type == FD_SERVER);
	if (isServer)
		handshake = SSL_connect(serv->fds[fd].sslptr);
	else
		handshake = SSL_accept(serv->fds[fd].sslptr);

	// continue if handshake need more actions (until it returns 1)
	if (handshake != 1)
	{
		// check if handshake need more actions or gone wrong by SSL_get_error
		int	err = SSL_get_error(serv->fds[fd].sslptr, handshake);
		// SSL_ERROR_WANT_READ/WRITE in case handshake needs another round
		if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE)
		{
			// drop the connection if handshake gone wrong
			std::cerr << "TLS handshake failed for client " << fd << std::endl;
			std::string		sslerr;
			ERR_print_errors_cb(SSLErrorCallback, &sslerr);
			if (isServer && sslerr.length() == 0)
			{
				sslerr = "TLS handshake failed for server ";
				sslerr += fd;
			}
			if (isServer)
				msg_error("SSL_connect: " + sslerr, serv);
			// we shouldn't call SSL_shutdown because it's already fatal
			SSL_free(serv->fds[fd].sslptr);
			close(fd);
			serv->fds.erase(fd);
		}
	}
}
