/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_connect.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:38:28 by salec             #+#    #+#             */
/*   Updated: 2020/11/12 22:29:53 by gbright          ###   ########.fr       */
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

/*SSL_CTX *InitSSL_CTX(void)
{
	const SSL_METHOD	*method;TLS_client_method();
	SSL_CTX				*ctx;
	
	method = TLS_client_method();
	if ((ctx = SSL_CTX_new(method)) == 0)
		error_exit("Error: ssl context creation error");
	return ctx;
}*/

void		do_connect(t_link &link, IRCserv *_server)
{
	int				socket_fd;
	t_sockaddr_in	server_addres;
	t_protoent		*protocol;
//	hostent			*host;

	if (!(protocol = getprotobyname("tcp"))) {
		msg_error("Can't resolve protocol by name while CONNECT", _server);
		return ;
	}
	if ((socket_fd = socket(PF_INET, SOCK_STREAM, protocol->p_proto) < 0)) {
		msg_error("Socket creating error while CONNECT", _server);
		return ;
	}
	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) < 0) {
		msg_error("fcntl error", _server);
		return ;
	}
	server_addres.sin_family = AF_INET;
	server_addres.sin_port = htons(link.port);
	if (!(server_addres.sin_addr.s_addr = inet_addr(link.ip.c_str()))) {
		msg_error("inet_addr error", _server);
		return ;
	}
	if (connect(socket_fd, (struct sockaddr*)&server_addres, sizeof(server_addres)) < 0) {
		msg_error("error while connecting to ", link.hostname, _server);
		return ;
	}
	_server->fds[socket_fd].type = FD_SERVER;
//	host = gethostbyname(link.hostname.c_str());
//	std::cout << std::endl << ">" << *(long *)(host->h_addr_list[0]) << "<" << std::endl << std::endl;
}

//CONNECT[0] <target server>[1] [<port>[2] [<remote server>][3]]
void		cmd_connect(int fd, const t_strvect &split, IRCserv *_server)
{
	//SSL_CTX	*ctx;
//	SSL		*ssl;
	size_t	i;

	do_connect(_server->link[0], _server); // DEL IT!
	if (fd != FD_ME)
	{
		std::vector<Client>::iterator	b = _server->clients.begin();
		std::vector<Client>::iterator	e = _server->clients.end();
		
		while (b != e)
		{
			if (b->getFD() == fd)
				break ;
			b++;
		}
		if (b == e || !b->isRegistred())
		{
			_server->fds[fd].wrbuf += get_reply(_server, ERR_NOTREGISTERED, -1, "",
					"You have not registered");
			return ;
		}
	}
	if (_server->fds[fd].type != FD_ME && _server->fds[fd].type != FD_OPER)
	{
		_server->fds[fd].wrbuf += get_reply(_server, ERR_NOPRIVILEGES, fd, "",
				"Permission Denied- You're not an IRC operator");
		return ;
	}
	i = -1;
	while (++i < _server->link.size())
		if (_server->link[i].hostname == split[1])
			break ;
	if (i == _server->link.size())
	{
		_server->fds[fd].wrbuf += get_reply(_server, ERR_NOSUCHSERVER, fd, split[1],
				"No such server");
		return ;
	}
	if (!(_server->link[i].tls))
		do_connect(_server->link[i], _server);
	std::cout << "here\n";
//	else
//		do_tls_connect(_server->link[i], _server);
	//ctx = InitSSL_CTX();
//	if ((ssl = SSL_new(ctx)) == 0)
//		error_exit("Error: SSL_new error");
}
