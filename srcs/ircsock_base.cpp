/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircsock_base.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/14 00:09:46 by salec             #+#    #+#             */
/*   Updated: 2020/11/14 20:37:06 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

void	ProcessMessage(int fd, std::string const &msg, IRCserv *serv)
{
	t_strvect	split = ft_splitstring(msg, " ");

	try
	{
		serv->command.at(split[0])(fd, split, serv);
		#if DEBUG_MODE
			std::cout << "command found:\t\t" << split[0] << std::endl;
		#endif
	}
	catch (std::out_of_range &e) { (void)e; }
}

void	CreateSock(IRCserv *serv)
{
	t_sockaddr_in	sockin;
	t_protoent		*pe = NULL;
	int				optval = 1;

	if (!(pe = getprotobyname("tcp")))
		error_exit("getprotobyname error");
	if ((serv->sock = socket(PF_INET, SOCK_STREAM, pe->p_proto)) < 0)
		error_exit("socket error");
	if (fcntl(serv->sock, F_SETFL, O_NONBLOCK) < 0)
		error_exit("fcntl error: failed to set nonblock fd");
	sockin.sin_family = AF_INET;
	sockin.sin_addr.s_addr = /*INADDR_ANY;*/ inet_addr("127.0.0.1");
	//	can change ip to create another local server without the b8s or
	//	containers just with ip variable seted in config file
	sockin.sin_port = htons(serv->port);
	if (setsockopt(serv->sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
		error_exit("set socket option returned error");
	if (bind(serv->sock, (t_sockaddr*)&sockin, sizeof(sockin)) < 0)
		error_exit("bind error (probably already binded)");
	if (listen(serv->sock, 42) < 0)
		error_exit("listen error");
	serv->fds[serv->sock].type = FD_ME;
	serv->fds[serv->sock].tls = false;
	std::cout << "server created on socket " << serv->sock <<
		" (port " << serv->port << ")" << std::endl;
}

void	AcceptConnect(IRCserv *serv, bool isTLS)
{
	int				fd;
	t_sockaddr_in	csin;
	socklen_t		csin_len;

	csin_len = sizeof(csin);
	if (isTLS)
		fd = accept(serv->tls_sock, (t_sockaddr*)&csin, &csin_len);
	else
		fd = accept(serv->sock, (t_sockaddr*)&csin, &csin_len);

	if (fd < 0)
		error_exit("accept error");
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		error_exit("fcntl error: failed to set nonblock fd");
	if (isTLS)
		std::cout << "tls";
	std::cout << "client " << fd << " accepted:\t" <<
		inet_ntoa(csin.sin_addr) << ":" << ntohs(csin.sin_port) << std::endl;
	serv->fds[fd].type = FD_CLIENT;
	// we dont know either it's client or other server
	serv->fds[fd].rdbuf.erase();
	serv->fds[fd].wrbuf.erase();
	serv->fds[fd].status = true;
	serv->fds[fd].tls = isTLS;
	serv->fds[fd].sslptr = NULL;

	if (isTLS)
	{
		if (!(serv->fds[fd].sslptr = SSL_new(serv->sslctx)))
		{
			ERR_print_errors_cb(SSLErrorCallback, NULL);
			error_exit("SSL_new failed");
		}
		if (SSL_set_fd(serv->fds[fd].sslptr, fd) < 1)
		{
			ERR_print_errors_cb(SSLErrorCallback, NULL);
			error_exit("SSL_set_fd failed");
		}
	}
}

void	ReceiveMessage(int fd, IRCserv *serv)
{
	ssize_t		r = 0;
	char		buf_read[BUF_SIZE + 1];

	if (serv->fds[fd].tls && serv->fds[fd].sslptr)
		r = SSL_read(serv->fds[fd].sslptr, buf_read, BUF_SIZE);
	else
		r = recv(fd, buf_read, BUF_SIZE, 0);

	if (r >= 0)
		buf_read[r] = 0;
	if (r > 0)
	{
		serv->fds[fd].rdbuf += buf_read;
		if (serv->fds[fd].rdbuf.rfind(CRLF) + serv->clrf.length() ==
			serv->fds[fd].rdbuf.length())
		{
#if DEBUG_MODE
			if (serv->fds[fd].tls)
				std::cout << "tls";
			std::cout << "client " << fd << " sent:\t" <<
				(serv->fds[fd].tls ? "" : "\t") << serv->fds[fd].rdbuf;
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
		if (serv->fds[fd].tls)
		{
			/* for tls may be recoverable */
			int	err = SSL_get_error(serv->fds[fd].sslptr, r);
			if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
				return ;
			ERR_print_errors_cb(SSLErrorCallback, NULL);
			SSL_free(serv->fds[fd].sslptr);
			std::cout << "tls";
		}
		close(fd);
		serv->fds.erase(fd);
		t_citer	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
		if (it != serv->clients.end())
			it->Disconnect();
		std::cout << "client " << fd << "\t\tdisconnected" << std::endl;
	}
}

void	SendMessage(int fd, IRCserv *serv)
{
	ssize_t		r = 0;
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
	if (serv->fds[fd].tls)
		std::cout << "tls";
	std::cout << "sending client " << fd << "\t" << reply;
#endif

	if (serv->fds[fd].tls && serv->fds[fd].sslptr)
		r = SSL_write(serv->fds[fd].sslptr, reply.c_str(), reply.length());
	else
		r = send(fd, reply.c_str(), reply.length(), 0);

	if (r <= 0 || serv->fds[fd].status == false)
	{
		if (serv->fds[fd].tls)
		{
			/* for tls may be recoverable */
			int	err = SSL_get_error(serv->fds[fd].sslptr, r);
			if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
				return ;
			ERR_print_errors_cb(SSLErrorCallback, NULL);
			SSL_free(serv->fds[fd].sslptr);
			std::cout << "tls";
		}
		close(fd);
		serv->fds.erase(fd);
		t_citer	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
		if (it != serv->clients.end())
			it->Disconnect();
		std::cout << "client " << fd << ":\t\tdisconnected" << std::endl;
	}
}
