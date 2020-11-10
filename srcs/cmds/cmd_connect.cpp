/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_connect.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:38:28 by salec             #+#    #+#             */
/*   Updated: 2020/11/10 21:08:48 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "message.hpp"
#include "commands.hpp"
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


//CONNECT[0] <target server>[1] [<port>[2] [<remote server>][3]]
void		cmd_connect(int fd, const t_strvect &split, IRCserv *_server)
{
	//SSL_CTX	*ctx;
//	SSL		*ssl;
	size_t	i;

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
		if (b == e)
		{
			_server->fds[fd].wrbuf += get_reply(_server, ERR_NOTREGISTERED, -1, "", "You have not registered");
			return ;
		}
	}
	if (_server->fds[fd].type != FD_ME && _server->fds[fd].type != FD_OPER)
	{
		_server->fds[fd].wrbuf += get_reply(_server, ERR_NOPRIVILEGES, fd, "", "Permission Denied- You're not an IRC operator");
		return ;
	}
	i = -1;
	while (++i < _server->link.size())
		if (_server->link[i].hostname == split[1])
			break ;
	if (i == _server->link.size())
	{
		std::cout << "ERR_NOSUCHSERVER\n";
		return ;
	}
	//ctx = InitSSL_CTX();
//	if ((ssl = SSL_new(ctx)) == 0)
//		error_exit("Error: SSL_new error");
	(void)split;

}
