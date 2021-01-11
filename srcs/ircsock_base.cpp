/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircsock_base.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/08 23:44:09 by gbright           #+#    #+#             */
/*   Updated: 2021/01/11 19:36:30 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "message.hpp"
#include "tools.hpp"

void	ProcessMessage(int fd, std::string const &msg, IRCserv *serv)
{
	t_strvect	split = ft_splitcmdbyspace(msg);
//	t_strvect	split = ft_splitstring(msg, " ");
//						^ old split just in case

	/*	reply check here: reroute the reply to user or server	*/
	//	serv->fds[fd].type = FD_ME not used because
	//	we get only connect accept requests on read of *this* server
	if (serv->fds[fd].type == FD_SERVER &&
		split.size() > 3 && split[1].size() == 3 &&
		split[1].find_first_not_of("0123456789") == std::string::npos)
	{
#if DEBUG_MODE
		std::cout << "A reply " << split[1] << " from fd " << fd <<
			" recieved. Forward to " << split[2] << std::endl;
#endif
		Client	*found = find_client_by_nick(split[2], serv);
		if (found)
		{
			try
			{
				serv->fds.at(found->getFD()).wrbuf += msg + CRLF;
				//	getFD() of client from another server is the FD of that
				//	other server we connected to (no need to do more here)
				//	just checking for existance with at()
			}
			catch (std::out_of_range &e) { (void)e; }
		}
		return ;
	}

	size_t	i = 0;
	if (split.size() > 0 && split[0][0] == ':')
		i = 1;
	if (i < split.size())
		split[i] = ft_strtoupper(split[i]);
	else
		return ;	// avoiding error if someone sends only prefix
	try
	{
		serv->cmds.at(split[i]).Execute(fd, split, serv, msg.size(), i > 0);
#if DEBUG_MODE
			std::cout << "command found:\t\t" << split[i] << std::endl;
#endif
	}
	catch (std::out_of_range &e)
	{
		(void)e;
		serv->fds[fd].wrbuf += reply_unknowncmd(fd, split, serv);
	}
}

void	CreateSock(IRCserv *serv, t_listen &_listen)
{
	t_sockaddr_in	sockin;
	t_protoent		*pe = NULL;
	int				optval = 1;

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
	if (listen(_listen.socket_fd, 42) < 0) //42 can also be configured
		error_exit("listen error");

	t_fd	&fdref = serv->fds[_listen.socket_fd];	// this will create fd
	fdref.type = FD_ME;
	fdref.tls = false;
	fdref.dtopened = ft_getcurrenttime();
	fdref.sentmsgs = 0;
	fdref.recvmsgs = 0;
	fdref.sentbytes = 0;
	fdref.recvbytes = 0;
	fdref.linkname = serv->servername +
		"[" + inet_ntoa(sockin.sin_addr) + ":" + TOSTRING(_listen.port) + "]";
#if DEBUG_MODE
	std::cout << "server created on socket " << _listen.socket_fd <<
		" (port " << _listen.port << ")" << std::endl;
#endif
}

void	AcceptConnect(int _socket, IRCserv *serv, bool isTLS)
{
	int				fd;
	t_sockaddr_in	csin;
	socklen_t		csin_len;

	csin_len = sizeof(csin);
	if (isTLS)
		fd = accept(_socket, (t_sockaddr*)&csin, &csin_len);
	else
		fd = accept(_socket, (t_sockaddr*)&csin, &csin_len);

	if (fd < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
		error_exit("accept error");
	else if (fd < 0)
	{
		errno = 0;
		return ;
	}
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		error_exit("fcntl error: failed to set nonblock fd");
#if DEBUG_MODE
	if (isTLS)
		std::cout << "tls";
	std::cout << "client " << fd << " accepted:\t" <<
		inet_ntoa(csin.sin_addr) << ":" << ntohs(csin.sin_port) << std::endl;
#endif

	t_fd	&fdref = serv->fds[fd];		// this will create t_fd and return ref
	fdref.type = FD_UNREGISTRED;
	// we dont know either it's client or other server
	fdref.rdbuf.erase();
	fdref.wrbuf.erase();
	fdref.blocked = false;
	fdref.status = true;
	fdref.tls = isTLS;
	fdref.hostname = inet_ntoa(csin.sin_addr);
	fdref.sslptr = NULL;
	fdref.dtopened = ft_getcurrenttime();
	fdref.sentmsgs = 0;
	fdref.recvmsgs = 0;
	fdref.sentbytes = 0;
	fdref.recvbytes = 0;
	fdref.file_size = 0;
	fdref.file_bytes_received = 0;
	fdref.sock = _socket;
	fdref.linkname = std::string("*[") +
		inet_ntoa(csin.sin_addr) + ":" + TOSTRING(ntohs(csin.sin_port)) + "]";

	if (isTLS)
	{
		if (!(fdref.sslptr = SSL_new(serv->sslctx)))
		{
			ERR_print_errors_cb(SSLErrorCallback, NULL);
			error_exit("SSL_new failed");
		}
		if (SSL_set_fd(fdref.sslptr, fd) < 1)
		{
			ERR_print_errors_cb(SSLErrorCallback, NULL);
			error_exit("SSL_set_fd failed");
		}
	}
}

void	self_cmd_squit(int fd, t_fd &fdref, IRCserv *serv)
{
	t_strvect	split;

	for (size_t i = 0; i < serv->network.size(); i++)
		if (serv->network[i].fd == fd)
		{
			split.push_back("SQUIT");
			split.push_back(serv->network[i].servername);
			split.push_back(":Read error");
			cmd_squit(serv->listen[0].socket_fd, split, serv);
			break ;
		}
	fdref.fatal = true;
}

void	self_cmd_quit(int fd, t_fd &fdref, IRCserv *serv)
{
	t_strvect	split;

	split.push_back("QUIT");
	split.push_back(":Read error");
	cmd_quit(fd, split, serv);
	fdref.fatal = true;
}

void	read_error(int fd, t_fd &fdref, ssize_t r, IRCserv *serv)
{
	if (errno == EAGAIN && errno == EWOULDBLOCK)
	{
		errno = 0;
		return ;
	}
	if (fdref.tls)
	{
		/* for tls may be recoverable */
		int	err = SSL_get_error(fdref.sslptr, r);
		if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
			return ;
		ERR_print_errors_cb(SSLErrorCallback, NULL);
	}
	if (fdref.type == FD_SERVER && !fdref.blocked && fdref.status)
		self_cmd_squit(fd, fdref, serv);
	else if ((fdref.type == FD_CLIENT || fdref.type == FD_OPER) && fdref.status)
		self_cmd_quit(fd, fdref, serv);
	if (!fdref.blocked && !fdref.status)
	{
		if (fdref.status)
			fdref.fatal = true;
		if (fdref.tls)
		{
			if (!fdref.fatal)
				SSL_shutdown(fdref.sslptr);
			SSL_free(fdref.sslptr);
#if DEBUG_MODE
			std::cout << "tls";
#endif
		}
		FD_CLR(fd, &(serv->fdset_read));
		FD_CLR(fd, &(serv->fdset_write));
		close(fd);
		serv->fds.erase(fd);
#if DEBUG_MODE
		std::cout << "client " << fd << ":\t\tdisconnected" << std::endl;
#endif
	}
}

void	ReceiveMessage(int fd, IRCserv *serv)
{
	ssize_t			r = 0;
	unsigned char	buf_read[BUF_SIZE + 1];
	t_fd			&fdref = serv->fds[fd];	// this will decrease amount of search

	if (fdref.tls && fdref.sslptr)
		r = SSL_read(fdref.sslptr, buf_read, BUF_SIZE);
	else
		r = recv(fd, buf_read, BUF_SIZE, 0);

	if (r >= 0)
		buf_read[r] = 0;
	if (r > 0)
	{
		fdref.recvbytes += r;
		if (fdref.sock > 0)
			serv->fds[fdref.sock].recvbytes += r;
		// ^ which sock recieved from fd
		fdref.rdbuf += (char*)buf_read;
		if (fdref.rdbuf.find_last_of(CRLF) != std::string::npos &&
			fdref.rdbuf.find_last_of(CRLF) + 1 == fdref.rdbuf.length())
		{
#if DEBUG_MODE
			if (fdref.tls)
				std::cout << "tls";
			std::cout << "client " << fd << " sent:\t" <<
				(fdref.tls ? "" : "\t") << fdref.rdbuf;
#endif
			t_strvect	split = ft_splitstringbyany(fdref.rdbuf, CRLF);
			fdref.recvmsgs += split.size();
			if (fdref.sock > 0)
				serv->fds[fdref.sock].recvmsgs += split.size();
			// ^ which sock recieved msgs from fd
			for (size_t i = 0; i < split.size(); i++)
				ProcessMessage(fd, split[i], serv);
			//	ignore msgs with \r\n (maybe other symbols too)
			//	if (split[i].find_first_of(CRLF) == std::string::npos)
			//	not much use here
			try { serv->fds.at(fd).rdbuf.erase(); }
			catch (std::out_of_range const &e) { (void)e; }
		}
	}
	else if (r <= 0 || !fdref.status)
		read_error(fd, fdref, r, serv);
}


void	SendMessage(int fd, IRCserv *serv)
{
	ssize_t		r = 0;
	std::string	reply;
	t_fd		&fdref = serv->fds[fd];
	bool		just_msg;

	just_msg = !fdref.wrbuf.empty();
	if (fdref.wrbuf.length() > BUF_SIZE)
	{
		reply = fdref.wrbuf.substr(0, BUF_SIZE);
		fdref.wrbuf = fdref.wrbuf.substr(BUF_SIZE);
	}
	else
	{
		reply = fdref.wrbuf;
		fdref.wrbuf.erase();
	}

	size_t tmp = ft_splitstringbyany(reply, CRLF).size();
	if (reply.find_last_of(CRLF) != reply.length())
		tmp--;
	fdref.sentmsgs += tmp;
	if (fdref.sock > 0)
		serv->fds[fdref.sock].sentmsgs += tmp;
	// ^ which sock sent this to fd

#if DEBUG_MODE
	if (fdref.tls)
		std::cout << "tls ";
	std::cout << "sending client " << fd << "\t" << reply;
#endif

	if (just_msg)
	{
		if (fdref.tls && fdref.sslptr)
			r = SSL_write(fdref.sslptr, reply.c_str(), reply.length());
		else
			r = send(fd, reply.c_str(), reply.length(), 0);
	}
	else
	{
		if (fdref.tls && fdref.sslptr)
			r = SSL_write(fdref.sslptr, fdref.file_buf.data(), fdref.file_buf.size());
		else
			r = send(fd, fdref.file_buf.data(), fdref.file_buf.size(), 0);
		fdref.file_buf.clear();
	}
	fdref.sentbytes += r;
	if (fdref.sock > 0)
		serv->fds[fdref.sock].sentbytes += r;
	// ^ which sock sent this to fd

	if (r <= 0 || fdref.status == false)
		read_error(fd, fdref, r, serv);
}

bool	didSockFail(int fd, IRCserv *serv)
{
	t_fd	&fdref = serv->fds[fd];

	if (fdref.status == false && fdref.type == FD_SERVER)
	{
		int			error;
		socklen_t	len = sizeof(error);
		if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
			error_exit("getsockopt failure");
		if (error)
		{
			std::cerr << "Connection error to server " << fd << std::endl;
			msg_error("Connection error to server", serv);
			close(fd);
			if (fdref.tls)
			{
				SSL_free(fdref.sslptr);
				serv->fds.erase(fd);
			}
			return (true);
		}
		else
			fdref.status = true;
	}
	return (false);
}
