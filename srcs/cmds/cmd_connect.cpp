/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_connect.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:38:28 by salec             #+#    #+#             */
/*   Updated: 2020/11/16 00:00:41 by gbright          ###   ########.fr       */
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

	int				socket_fd;
	t_sockaddr_in	server_addres;
	t_protoent		*protocol;
//	hostent			*host;

	if (!(protocol = getprotobyname("tcp"))) {
		msg_error("Can't resolve protocol by name while CONNECT", serv);
		return ;
	}
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, protocol->p_proto)) < 0) {
		msg_error("Socket creating error while CONNECT", serv);
		return ;
	}
	bzero(&server_addres, sizeof(server_addres));
	server_addres.sin_family = AF_INET;
	server_addres.sin_port = htons(link.port);
	if (!(server_addres.sin_addr.s_addr = inet_addr(link.ip.c_str()))) {
		msg_error("inet_addr error", serv);
		return ;
	}
	if (connect(socket_fd, (struct sockaddr*)&server_addres, sizeof(server_addres)) < 0) {
		msg_error("error while connecting to ", link.hostname, serv);
		return ;
	}
	serv->fds[socket_fd].type = FD_SERVER;
	serv->fds[socket_fd].status = true;
	if (link.pass != "")
		serv->fds[socket_fd].wrbuf = "PASS " + link.pass + CRLF;
	serv->fds[socket_fd].wrbuf += "SERVER " + serv->hostname + " 0 " +
		serv->token + " " + serv->info + CRLF;
	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) < 0) {
		msg_error("fcntl error", serv);
		return ;
	}
//	host = gethostbyname(link.hostname.c_str());
//	std::cout << std::endl << ">" << *(long *)(host->h_addr_list[0]) << "<" << std::endl << std::endl;
	return ;
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
		if (serv->link[i].hostname == split[1])
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
