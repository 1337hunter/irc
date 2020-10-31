/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 16:44:15 by salec             #+#    #+#             */
/*   Updated: 2020/10/31 16:34:54 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

typedef struct addrinfo		t_addrinfo;
std::string const			IRCserv::clrf = CLRF;

void		CreateSock(IRCserv *_server)
{
	t_sockaddr_in	sockin;
	t_protoent		*pe = NULL;
	int				optval = 1;

	if (!(pe = getprotobyname("tcp")))
		error_exit("getprotobyname error");
	if ((_server->sock = socket(PF_INET, SOCK_STREAM, pe->p_proto)) < 0)
		error_exit("socket error");
	if (fcntl(_server->sock, F_SETFL, O_NONBLOCK) < 0)
		error_exit("fcntl error: failed to set nonblock fd");
	sockin.sin_family = AF_INET;
	sockin.sin_addr.s_addr = INADDR_ANY;// inet_addr("127.0.0.1"); //can changee ip
	// to create another local server without the b8s or containers just with ip variable seted in config file
	sockin.sin_port = htons(_server->port);
	if (setsockopt(_server->sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
		error_exit("set socket option returned error");
	if (bind(_server->sock, (t_sockaddr*)&sockin, sizeof(sockin)) < 0)
		error_exit("bind error (probably already binded)");
	if (listen(_server->sock, 42) < 0)
		error_exit("listen error");
	_server->fds[_server->sock].type = FD_SERVER;
#if DEBUG_MODE
	std::cout << "Server created on sock " << _server->sock << std::endl;
	t_addrinfo		*ai = NULL;
	t_addrinfo		hints = {};
	hints.ai_family = AF_INET;			// AF_UNSPEC to get v4 and v6
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (getaddrinfo("127.0.0.1", "6667", &hints, &ai) < 0)
		error_exit("getaddrinfo error");
	std::cout << "ai addr:\t" << ai->ai_addr << std::endl <<
		"ai addrlen:\t" << ai->ai_addrlen << std::endl <<
		"ai canonname:\t" <<
		(ai->ai_canonname ? ai->ai_canonname : "(null)") << std::endl <<
		"ai flags:\t" << ai->ai_flags << std::endl <<
		"ai next:\t" << ai->ai_next << std::endl;
	freeaddrinfo(ai);
	t_sockaddr		sa;
	socklen_t		salen = sizeof(t_sockaddr);
	if (getsockname(_server->sock, &sa, &salen) < 0)
		error_exit("getsockname error");
#endif
	_server->hostname = "localhost";	// need to get it properly later
}

void		AcceptConnect(IRCserv *_server)
{
	int				fd;
	t_sockaddr_in	csin;
	socklen_t		csin_len;

	csin_len = sizeof(csin);
	if ((fd = accept(_server->sock, (t_sockaddr*)&csin, &csin_len)) < 0)
		error_exit("accept error");
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		error_exit("fcntl error: failed to set nonblock fd");
#if DEBUG_MODE
	std::cout << "client " << fd << " from " <<
		inet_ntoa(csin.sin_addr) << ":\t" << ntohs(csin.sin_port) <<
		" accepted" << std::endl;
#endif
	_server->fds[fd].type = FD_CLIENT; // we dont know either it's client or other server
	_server->fds[fd].rdbuf.erase();
}

void		ProcessMessage(int fd, std::string const &msg, IRCserv *_server)
{
	t_strvect	split = ft_splitstring(msg, " ");

#if DEBUG_MODE
	std::cout << "command received:\t\t";
	for (t_strvect::iterator it = split.begin(); it != split.end(); it++)
		std::cout << *it << ' ';
	std::cout << std::endl;
#endif
	try
	{
		_server->command.at(split[0])(fd, split, _server);
#if DEBUG_MODE
		std::cout << "command found:\t\t" << "|" << split[0] << "|" << std::endl;
#endif
	}
	catch (std::out_of_range &e)
	{
		(void)e;
	}
}

void		RecieveMessage(int fd, IRCserv *_server)
{
	ssize_t		r;
	char		buf_read[BUF_SIZE + 1];

	if ((r = recv(fd, buf_read, BUF_SIZE, 0)) >= 0)
		buf_read[r] = 0;
	if (r > 0)
	{
		_server->fds[fd].rdbuf += buf_read;
		if (_server->fds[fd].rdbuf.rfind(CLRF) + _server->clrf.length() ==
			_server->fds[fd].rdbuf.length())
		{
#if DEBUG_MODE
			std::cout << "client " << fd << " sent:\t\t\t" << _server->fds[fd].rdbuf;
#endif
			t_strvect	split = ft_splitstring(_server->fds[fd].rdbuf, CLRF);
			for (size_t i = 0; i < split.size(); i++)
				ProcessMessage(fd, split[i], _server);
			try
			{
				_server->fds.at(fd).rdbuf.erase();
			}
			catch (std::out_of_range const &e)
			{
				(void)e;
			}
		}
	}
	else
	{
		close(fd);
		_server->fds.erase(fd);
		t_citer	it = ft_findclientfd(_server->clients.begin(), _server->clients.end(), fd);
		if (it != _server->clients.end())
			it->Disconnect();
#if DEBUG_MODE
		std::cout << "client " << fd << ":\t\t\tdisconnected" << std::endl;
#endif
	}
}

void		RunServer(IRCserv *_server)
{
	CreateSock(_server);
	while (1)
	{
		int	lastfd = 0;
		FD_ZERO(&(_server->fdset_read));
		for (std::map<int, t_fd>::iterator it = _server->fds.begin();
			it != _server->fds.end(); it++)
		{
			FD_SET(it->first, &(_server->fdset_read));
			lastfd = std::max(lastfd, it->first);
		}
		int readyfds = select(lastfd + 1, &(_server->fdset_read), NULL, NULL, NULL);
		if (readyfds < 0)
			error_exit("select error");
		for (std::map<int, t_fd>::iterator it = _server->fds.begin();
			readyfds > 0 && it != _server->fds.end(); it++)
		{
			if (FD_ISSET(it->first, &(_server->fdset_read)))
			{
				if (_server->fds[it->first].type == FD_SERVER)
					AcceptConnect(_server);
				else if (_server->fds[it->first].type == FD_CLIENT)
					RecieveMessage(it->first, _server);
				readyfds--;
			}
		}
	}
}
