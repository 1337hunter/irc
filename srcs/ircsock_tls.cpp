
#include "ircserv.hpp"

void	InitSSLCTX(IRCserv *serv)
{
	SSL_library_init();
	if (!(serv->sslctx = SSL_CTX_new(TLS_server_method())))
		error_exit("Unable to create SSL context");
	if (SSL_CTX_set_ecdh_auto(ctx, 1) <= 0)
		error_exit("SSL_CTX_set_ecdh_auto failed");

	/* Set the key and cert */
	if (SSL_CTX_use_certificate_file(serv->sslctx,
		"./conf/ircserv.crt", SSL_FILETYPE_PEM) <= 0)
		error_exit("Failed to load a certificate");
	if (SSL_CTX_use_PrivateKey_file(serv->sslctx,
		"./conf/ircserv.key", SSL_FILETYPE_PEM) <= 0)
		error_exit("Failed to load a private key");
}

void	CreateSockTLS(IRCserv *serv)
{
	t_sockaddr_in	sockin;
	t_protoent		*pe = NULL;
	int				optval = 1;

	InitSSLCTX(serv);

	if (!(pe = getprotobyname("tcp")))
		error_exit("getprotobyname error");
	if ((serv->sock_tls = socket(PF_INET, SOCK_STREAM, pe->p_proto)) < 0)
		error_exit("socket error");
	if (fcntl(serv->sock_tls, F_SETFL, O_NONBLOCK) < 0)
		error_exit("fcntl error: failed to set nonblock fd");

	sockin.sin_family = AF_INET;
	sockin.sin_addr.s_addr = /*INADDR_ANY;*/ inet_addr("127.0.0.1");
	//	can change ip to create another local server without the b8s or
	//	containers just with ip variable seted in config file
	sockin.sin_port = htons(serv->port_tls);

	if (setsockopt(serv->sock_tls, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
		error_exit("set socket option returned error");
	if (bind(serv->sock_tls, (t_sockaddr*)&sockin, sizeof(sockin)) < 0)
		error_exit("bind error (probably already binded)");
	if (listen(serv->sock_tls, 42) < 0)
		error_exit("listen error");
	serv->fds[serv->sock_tls].type = FD_ME;
	serv->fds[serv->sock_tls].tls = true;
	std::cout << "server created on socket " << serv->sock_tls << std::endl;
}

void	AcceptConnectTLS(IRCserv *serv)
{
	int				fd;
	t_sockaddr_in	csin;
	socklen_t		csin_len;

	csin_len = sizeof(csin);
	if ((fd = accept(serv->sock_tls, (t_sockaddr*)&csin, &csin_len)) < 0)
		error_exit("accept error");
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		error_exit("fcntl error: failed to set nonblock fd");
	std::cout << "tls client " << fd << " accepted from\t" <<
		inet_ntoa(csin.sin_addr) << ":" << ntohs(csin.sin_port) << std::endl;
	serv->fds[fd].type = FD_CLIENT;
	// we dont know either it's client or other server
	serv->fds[fd].rdbuf.erase();
	serv->fds[fd].wrbuf.erase();
	serv->fds[fd].status = true;
	serv->fds[fd].tls = true;
}

void	ReceiveMessageTLS(int fd, IRCserv *serv)
{
	ssize_t		r;
	char		buf_read[BUF_SIZE + 1];

	if ((r = recv(fd, buf_read, BUF_SIZE, 0)) >= 0)
		buf_read[r] = 0;
	if (r > 0)
	{
		serv->fds[fd].rdbuf += buf_read;
		if (serv->fds[fd].rdbuf.rfind(CRLF) + serv->clrf.length() ==
			serv->fds[fd].rdbuf.length())
		{
#if DEBUG_MODE
			std::cout << "client " << fd << " sent:\t\t" << serv->fds[fd].rdbuf;
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
		close(fd);
		serv->fds.erase(fd);
		t_citer	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
		if (it != serv->clients.end())
			it->Disconnect();
		std::cout << "client " << fd << "\t\tdisconnected" << std::endl;
	}
}

void	SendMessageTLS(int fd, IRCserv *serv)
{
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
	std::cout << "sending client " << fd << "\t" << reply;
#endif
	ssize_t		r = send(fd, reply.c_str(), reply.length(), 0);
	if (r <= 0 || serv->fds[fd].status == false)
	{
		close(fd);
		serv->fds.erase(fd);
		t_citer	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
		if (it != serv->clients.end())
			it->Disconnect();
		std::cout << "client " << fd << ":\t\tdisconnected" << std::endl;
	}
}