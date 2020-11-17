/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_connect.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:38:28 by salec             #+#    #+#             */
/*   Updated: 2020/11/17 16:05:57 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "message.hpp"
#include "commands.hpp"
#include <sys/socket.h>
#include "error_handle.hpp"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netdb.h>
#include <resolv.h>
#include <cstring>



SSL_CTX *InitSSL_CTX(void)
{
	const SSL_METHOD	*method;TLS_client_method();
	SSL_CTX				*ctx;
	
	method = TLS_client_method();
	if ((ctx = SSL_CTX_new(method)) == 0)
		error_exit("Error: ssl context creation error");
	return ctx;
}

void	do_tls_connect(t_link, IRCserv *serv)
{
	SSL_CTX	*ctx;
	SSL		*ssl;
//	X509	*cert;

	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();
	ctx = InitSSL_CTX();
	if ((ssl = SSL_new(ctx)) == 0)
		error_exit("Error: SSL_new error");
	serv = 0;
}

void	do_connect(t_link &link, IRCserv *serv)
{

	int					socket_fd;
	struct	addrinfo	hints;
	struct	addrinfo	*addrs;

	hints.ai_flags = 0;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (getaddrinfo(link.hostname.c_str(), (std::to_string(link.port)).c_str(), &hints, &addrs))
	{
		msg_error("Can't get addres information with getaddrinfo", serv);
		return ;
	}
	struct addrinfo *addr = addrs;
	for (;addr != 0; addr = addr->ai_next)
	{

		if ((socket_fd = socket(addr->ai_family, SOCK_STREAM, addr->ai_protocol)) < 0)
			continue ;
		if (connect(socket_fd, addr->ai_addr, addr->ai_addrlen) == 0)
			break ;
	}
	if (socket_fd < 0)
	{
		freeaddrinfo(addrs);
		msg_error("Socket error while server link", serv);
		return ;
	}
	if (addr == 0)
	{
		freeaddrinfo(addrs);
		msg_error("Connection error while server link", serv);
		return ;
	}
	serv->fds[socket_fd].type = FD_SERVER;
	serv->fds[socket_fd].status = true;
	if (link.pass.size() != 0)
		serv->fds[socket_fd].wrbuf = "PASS " + link.pass + CRLF;
	serv->fds[socket_fd].wrbuf += "SERVER " + serv->hostname + " 0 " +
		serv->token + " " + serv->info + CRLF;
	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) < 0) {
		freeaddrinfo(addrs);
		msg_error("fcntl error", serv);
		return ;
	}
	freeaddrinfo(addrs);
}

//CONNECT[0] <target server>[1] [<port>[2] [<remote server>][3]]
void		cmd_connect(int fd, const t_strvect &split, IRCserv *serv)
{
	size_t	i;

	if (fd != FD_ME)
	{
		std::vector<Client>::iterator	b = serv->clients.begin();
		std::vector<Client>::iterator	e = serv->clients.end();
		
		while (b != e)
		{
			if (b->getFD() == fd)
				break ;
			b++;
		}
		if (b == e || !b->isRegistred())
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1, "",
					"You have not registered");
			return ;
		}
	}
	if (serv->fds[fd].type != FD_ME && serv->fds[fd].type != FD_OPER)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOPRIVILEGES, fd, "",
				"Permission Denied- You're not an IRC operator");
		return ;
	}
	i = -1;
	while (++i < serv->link.size())
		if (serv->link[i].servername == split[1])
			break ;
	if (i == serv->link.size())
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHSERVER, fd, split[1],
				"No such server");
		return ;
	}
	if (!(serv->link[i].tls))
		do_connect(serv->link[i], serv);
	else
		do_tls_connect(serv->link[i], serv);
}
