/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircsock_tls.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/14 02:03:53 by salec             #+#    #+#             */
/*   Updated: 2020/11/14 17:50:44 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include <openssl/err.h>

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

	/* init ssl lib and ctx */
	SSL_library_init();
	SSL_load_error_strings();
	/* ERR_free_strings() may be needed if we want to cleanup memory */
	if (!(serv->sslctx = SSL_CTX_new(TLS_server_method())))
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
	return (0);
}

void	CreateSockTLS(IRCserv *serv)
{
	t_sockaddr_in	sockin;
	t_protoent		*pe = NULL;
	int				optval = 1;

	if (InitSSLCTX(serv))
		return ;

	if (serv->port + 30 > 65535)
		serv->tls_port = serv->port - 30;
	else
		serv->tls_port = serv->port + 30;	// temp tls port = basic port + 30

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
	std::cout << "tlsserver created on socket " << serv->tls_sock <<
		" (port " << serv->tls_port << ")" << std::endl;
}

void	DoHandshakeTLS(int fd, IRCserv *serv)
{
	int	handshake = SSL_accept(serv->fds[fd].sslptr);

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
			ERR_print_errors_cb(SSLErrorCallback, NULL);
			SSL_shutdown(serv->fds[fd].sslptr);
			SSL_free(serv->fds[fd].sslptr);
			close(fd);
			serv->fds.erase(fd);
		}
	}
}
