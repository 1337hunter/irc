/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_connect.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:38:28 by salec             #+#    #+#             */
/*   Updated: 2020/11/28 10:54:39 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "message.hpp"
#include "commands.hpp"
#include "error_handle.hpp"

typedef struct addrinfo		t_addrinfo;

int		do_connect(t_link &link, IRCserv *serv, bool tls = false)
{
	int			socket_fd;
	t_addrinfo	hints;
	t_addrinfo	*addr;
	t_server	_server;

	hints.ai_flags = 0;
	hints.ai_family = AF_INET;			// AF_UNSPEC maybe can work idk
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		msg_error("Socket error while server link", serv); return socket_fd; }
	else if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) < 0) {
		msg_error("fcntl error", serv); return -1; }
	if (getaddrinfo(link.hostname.c_str(), (std::to_string(link.port)).c_str(), &hints, &addr)) {
		msg_error("Can't get addres information with getaddrinfo", serv); return -1; }
	int	res = connect(socket_fd, addr->ai_addr, addr->ai_addrlen);
	// this is temporary
	freeaddrinfo(addr);
	// gotta figure out how to check all addresses
	if (res == 0)
		serv->fds[socket_fd].status = true;
	else if (res == -1 && errno == EINPROGRESS)
	{
		serv->fds[socket_fd].status = false;
		errno = 0;
	}
	else
	{
		msg_error("Connection error while server link", serv);
		serv->fds.erase(socket_fd);
		close(socket_fd);
		errno = 0;
		return (-1);
	}
	if (tls)
		return (socket_fd);
	serv->fds[socket_fd].type = FD_SERVER;
	if (link.pass.length() != 0) // I think pass is necessery.
		serv->fds[socket_fd].wrbuf = "PASS " + link.pass + " " + serv->version +
		   	" " + "|" + CRLF;//PZ
	serv->fds[socket_fd].wrbuf += "SERVER " + serv->servername + " 1 " +
		serv->token + " " + ":" + serv->info + CRLF; //attempt to register
	serv->fds[socket_fd].tls = false;
	return socket_fd;
}

void	do_tls_connect(t_link &link, IRCserv *serv)
{
	SSL			*ssl;
	int			socket_fd;
	std::string	sslerr;
	t_server	_server;

	if (serv->sslctx == NULL)
	{
		msg_error("Error: No ssl context to create connection", serv);
		return ;
	}
	if ((ssl = SSL_new(serv->sslctx)) == 0)
	{
		ERR_print_errors_cb(SSLErrorCallback, &sslerr);
		msg_error("SSL_new: " + sslerr, serv);
		return ;
	}
	if ((socket_fd = do_connect(link, serv, true)) < 0)
	{
		msg_error("Socket error while server link", serv);
		return ;
	}
	if (!(SSL_set_fd(ssl, socket_fd)))
	{
		ERR_print_errors_cb(SSLErrorCallback, &sslerr);
		msg_error("SSL_set_fd: " + sslerr, serv);
		return ;
	}
	serv->fds[socket_fd].type = FD_SERVER;
	if (link.pass.length() != 0) // I think pass is necessery.
		serv->fds[socket_fd].wrbuf = "PASS " + link.pass + " " + serv->version +
		   	" " + "|" + CRLF;//P option?
	serv->fds[socket_fd].wrbuf += "SERVER " + serv->servername + " 1 " +
		serv->token + " " + ":" + serv->info + CRLF; //attempt to register
	serv->fds[socket_fd].tls = true;
	serv->fds[socket_fd].sslptr = ssl;
}

//CONNECT[0] <target server>[1] [<port>[2] [<remote server>][3]]
void		cmd_connect(int fd, const t_strvect &split, IRCserv *serv)
{
	size_t	i;

	if (fd != FD_ME)
	{
		std::list<Client>::iterator	b = serv->clients.begin();
		std::list<Client>::iterator	e = serv->clients.end();

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
	if (i == serv->link.size()) {
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHSERVER, fd, split[1],
				"No such server"); return ; }
	if (!(serv->link[i].tls))
		do_connect(serv->link[i], serv);
	else
		do_tls_connect(serv->link[i], serv);
}
