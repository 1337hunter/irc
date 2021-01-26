#include "ircserv.hpp"
#include "message.hpp"
#include "tools.hpp"

void	ProcessMessage(int fd, std::string const &msg, IRCserv *serv)
{
	t_strvect	split = ft_splitcmdbyspace(msg);

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
		return ;
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
	if (listen(_listen.socket_fd, 42) < 0)
		error_exit("listen error");

	t_fd	&fdref = serv->fds[_listen.socket_fd];
	fdref.type = FD_ME;
	fdref.status = true;
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
	socklen_t		csin_len = sizeof(csin);

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

	t_fd	&fdref = serv->fds[fd];
	fdref.type = FD_UNREGISTRED;
	fdref.status = true;
	fdref.tls = isTLS;
	fdref.hostname = inet_ntoa(csin.sin_addr);
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

void	read_error(int fd, t_fd &fdref, ssize_t r, IRCserv *serv)
{
	if (errno == EAGAIN && errno == EWOULDBLOCK)
	{
		errno = 0;
		return ;
	}
	if (fdref.tls)
	{
		int	err = SSL_get_error(fdref.sslptr, r);
		if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
			return ;
		ERR_print_errors_cb(SSLErrorCallback, NULL);
	}
	if (fdref.type == FD_SERVER && !fdref.blocked && fdref.status)
	{
		self_cmd_squit(fd, fdref, serv);
		fdref.fatal = true;
	}
	else if ((fdref.type == FD_CLIENT || fdref.type == FD_OPER) && fdref.status)
	{
		self_cmd_quit(fd, fdref, serv, "Read error");
		fdref.fatal = true;
	}
	else if (fdref.type == FD_SERVICE && fdref.status)
	{
		self_service_quit(fd, fdref, serv);
		fdref.fatal = true;
	}
	else if (fdref.type == FD_UNREGISTRED && fdref.status)
	{
		fdref.fatal = true;
		fdref.status = false;
	}

	if (!fdref.blocked && !fdref.status)
	{
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
	t_fd			&fdref = serv->fds[fd];

	if (fdref.tls && fdref.sslptr)
		r = SSL_read(fdref.sslptr, buf_read, BUF_SIZE);
	else
		r = recv(fd, buf_read, BUF_SIZE, 0);

	if (r >= 0)
		buf_read[r] = 0;

	if (r <= 0 || fdref.status == false)
		read_error(fd, fdref, r, serv);
	else
	{
		fdref.recvbytes += r;
		if (fdref.sock > 0)
			serv->fds[fdref.sock].recvbytes += r;
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
			fdref.lastactive = ft_getcurrenttime();
			t_strvect	split = ft_splitstringbyany(fdref.rdbuf, CRLF);
			fdref.recvmsgs += split.size();
			if (fdref.sock > 0)
				serv->fds[fdref.sock].recvmsgs += split.size();
			for (size_t i = 0; i < split.size(); i++)
				ProcessMessage(fd, split[i], serv);
			try { serv->fds.at(fd).rdbuf.erase(); }
			catch (std::out_of_range const &e) { (void)e; }
		}
	}
}

void	SendMessage(int fd, IRCserv *serv)
{
	ssize_t		r = 0;
	std::string	reply, remain;
	t_fd		&fdref = serv->fds[fd];

	if (fdref.wrbuf.length() > BUF_SIZE)
	{
		reply = fdref.wrbuf.substr(0, BUF_SIZE);
		remain = fdref.wrbuf.substr(BUF_SIZE);
	}
	else
	{
		reply = fdref.wrbuf;
		remain = "";
	}

#if DEBUG_MODE
	if (fdref.tls && !reply.empty())
		std::cout << "tls ";
	if (!reply.empty())
		std::cout << "sending client " << fd << "\t" << reply;
#endif

	if (fdref.tls && fdref.sslptr)
		r = SSL_write(fdref.sslptr, reply.c_str(), reply.length());
	else
		r = send(fd, reply.c_str(), reply.length(), 0);

	if (r <= 0 || fdref.status == false)
		read_error(fd, fdref, r, serv);
	else
	{
		size_t msgs = ft_splitstringbyany(reply, CRLF).size();
		if (reply.find_last_of(CRLF) != reply.length() - 1)
			msgs--;

		fdref.sentmsgs += msgs;
		if (fdref.sock > 0)
			serv->fds[fdref.sock].sentmsgs += msgs;

		fdref.sentbytes += r;
		if (fdref.sock > 0)
			serv->fds[fdref.sock].sentbytes += r;

		fdref.wrbuf = remain;
	}
}

t_fd::s_fd() : type(FD_FREE), tls(false),
		status(false), fatal(false), blocked(false),
		sslptr(NULL), lastactive(ft_getcurrenttime()),
		awaitingpong(false), sock(-1), sentmsgs(0), recvmsgs(0),
		sentbytes(0), recvbytes(0), dtopened(this->lastactive) {}

/*
bool	didSockFail(int fd, IRCserv *serv)
{
	t_fd	&fdref = serv->fds[fd];

	if (fdref.inprogress == true && fdref.status == false &&
		fdref.blocked == false && fdref.type == FD_SERVER)
	{
		int			error;
		socklen_t	len = sizeof(error);
		if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
			error_exit("getsockopt failure");
		if (error)
		{
#if DEBUG_MODE
			std::cerr << "Connection error to server " << fd << std::endl;
#endif
			msg_error("Connection error to server " + fdref.linkname, serv);
			if (fdref.tls)
				SSL_free(fdref.sslptr);
			FD_CLR(fd, &(serv->fdset_read));
			FD_CLR(fd, &(serv->fdset_write));
			close(fd);
			serv->fds.erase(fd);
#if DEBUG_MODE
			std::cout << "server " << fd << ":\t\tdisconnected" << std::endl;
#endif
			return (true);
		}
		else
		{
			fdref.status = true;
			fdref.inprogress = false;
		}
	}
	return (false);
}
*/
