/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 16:44:15 by salec             #+#    #+#             */
/*   Updated: 2020/11/04 19:09:18 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

std::string const	IRCserv::clrf = CLRF;

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
#endif
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
	std::cout << "client " << fd << " accepted from\t" <<
		inet_ntoa(csin.sin_addr) << ":" << ntohs(csin.sin_port) << std::endl;
#endif
	_server->fds[fd].type = FD_CLIENT; // we dont know either it's client or other server
	_server->fds[fd].rdbuf.erase();
	_server->fds[fd].wrbuf.erase();
}

void		ProcessMessage(int fd, std::string const &msg, IRCserv *_server)
{
	t_strvect	split = ft_splitstring(msg, " ");

	try
	{
		_server->command.at(split[0])(fd, split, _server);
#if DEBUG_MODE
		std::cout << "command found:\t\t" << split[0] << std::endl;
#endif
	}
	catch (std::out_of_range &e) { (void)e; }
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
			std::cout << "client " << fd << " sent:\t\t" << _server->fds[fd].rdbuf;
#endif
			t_strvect	split = ft_splitstring(_server->fds[fd].rdbuf, CLRF);
			for (size_t i = 0; i < split.size(); i++)
				ProcessMessage(fd, split[i], _server);
			try { _server->fds.at(fd).rdbuf.erase(); }
			catch (std::out_of_range const &e) { (void)e; }
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
		std::cout << "client " << fd << "\t\tdisconnected" << std::endl;
#endif
	}
}

void		SendMessage(int fd, IRCserv *_server)
{
	std::string	reply;

	if (_server->fds[fd].wrbuf.length() > BUF_SIZE)
	{
		reply = _server->fds[fd].wrbuf.substr(0, BUF_SIZE);
		_server->fds[fd].wrbuf.substr(BUF_SIZE);
	}
	else
	{
		reply = _server->fds[fd].wrbuf;
		_server->fds[fd].wrbuf.erase();
	}
#if DEBUG_MODE
		std::cout << "sending client " << fd << "\t" << reply;
#endif
	ssize_t		r = send(fd, reply.c_str(), reply.length(), 0);
	if (r <= 0)
	{
		close(fd);
		_server->fds.erase(fd);
		t_citer	it = ft_findclientfd(_server->clients.begin(), _server->clients.end(), fd);
		if (it != _server->clients.end())
			it->Disconnect();
#if DEBUG_MODE
		std::cout << "client " << fd << ":\t\tdisconnected" << std::endl;
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
		FD_ZERO(&(_server->fdset_write));
		for (std::map<int, t_fd>::iterator it = _server->fds.begin();
			it != _server->fds.end(); it++)
		{
			FD_SET(it->first, &(_server->fdset_read));
			if (!(it->second.wrbuf.empty()))
				FD_SET(it->first, &(_server->fdset_write));
			lastfd = std::max(lastfd, it->first);
		}
		int readyfds = select(lastfd + 1,
			&(_server->fdset_read), &(_server->fdset_write), NULL, NULL);
		if (readyfds < 0)
			error_exit("select error");
		size_t tmpits = 0;
		// BUG: iterators become invalid after disconnect or erase
		// possibly not good solution is to check how many fds we processed
		// may segv if multiple clients disconnect at the same time
		for (std::map<int, t_fd>::iterator it = _server->fds.begin();
			readyfds > 0 && tmpits < _server->fds.size() &&
			it != _server->fds.end(); it++)
		{
			int		tmpfd = it->first;
			if (FD_ISSET(tmpfd, &(_server->fdset_read)))
			{
				if (_server->fds[tmpfd].type == FD_SERVER)
					AcceptConnect(_server);
				else if (_server->fds[tmpfd].type == FD_CLIENT)
					RecieveMessage(tmpfd, _server);
			}
			if (FD_ISSET(tmpfd, &(_server->fdset_write)))
			{
				if (_server->fds[tmpfd].type == FD_CLIENT)
					SendMessage(tmpfd, _server);
			}
			if (FD_ISSET(tmpfd, &(_server->fdset_read)) ||
				FD_ISSET(tmpfd, &(_server->fdset_write)))
				readyfds--;
			tmpits++;
		}
	}
}
