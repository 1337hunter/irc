/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 16:44:15 by salec             #+#    #+#             */
/*   Updated: 2020/10/27 14:50:48 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

typedef	std::vector<Client>::iterator	t_citer;
std::string const	IRCserv::clrf = CLRF;

t_citer		ft_findclientfd(t_citer const &begin, t_citer const &end, int fd)
{
	for (t_citer it = begin; it != end; it++)
		if (it->isConnected() && it->getFD() == fd)
			return (it);
	return (end);
}

t_citer		ft_findnick(t_citer const &begin, t_citer const &end, std::string const &nick)
{
	for (t_citer it = begin; it != end; it++)
		if (it->getnickname() == nick)
			return (it);
	return (end);
}

void		CreateSock(IRCserv *_server)
{
	t_sockaddr_in	sockin;
	t_protoent		*pe;

	if (!(pe = getprotobyname("tcp")))
		error_exit("getprotobyname error");
	if ((_server->sock = socket(PF_INET, SOCK_STREAM, pe->p_proto)) < 0)
		error_exit("socket error");
	if (fcntl(_server->sock, F_SETFL, O_NONBLOCK) < 0)
		error_exit("fcntl error: failed to set nonblock fd");
	sockin.sin_family = AF_INET;
	sockin.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1"); can changee ip
	// to create another local server without the b8s or containers just with ip variable seted in config file
	sockin.sin_port = htons(_server->port);
	if (bind(_server->sock, (t_sockaddr*)&sockin, sizeof(sockin)) < 0)
		error_exit("bind error (probably already binded)");
	if (listen(_server->sock, 42) < 0)
		error_exit("listen error");
	_server->fds[_server->sock].type = FD_SERVER;
	std::cout << "Server created on sock " << _server->sock << std::endl;
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
	std::cout << "Client " << fd << " from " <<
		inet_ntoa(csin.sin_addr) << ":" << ntohs(csin.sin_port) <<
		" accepted" << std::endl;
	_server->fds[fd].type = FD_CLIENT;
	_server->fds[fd].rdbuf.erase();
}

void		ProcessMessage(int fd, std::string const &msg, IRCserv *_server)
{
	t_strvect		split = ft_splitstring(msg, " ");
	t_citer			it;
	std::string		reply;

	if (split[0] == "NICK")
	{
		if (ft_findnick(_server->clients.begin(), _server->clients.end(), split[1]) ==
			_server->clients.end())
			_server->clients.push_back(Client(split[1], fd));
		else
		{
			reply = ":localhost ";
			reply += ERR_NICKNAMEINUSE;
			reply += " " + split[1] + " :Nickname is already in use" + CLRF;
			send(fd, reply.c_str(), reply.length(), 0);
			/*	need to save the state in this case
				cli will try to send another NICK after USER msg	*/
		}
	}
	else if (split[0] == "USER")
	{
		it = ft_findclientfd(_server->clients.begin(), _server->clients.end(), fd);
		if (it != _server->clients.end())
		{
			it->Register(split[1], split[4]);
			reply = ":localhost ";
			reply += RPL_WELCOME;
			reply += " " + it->getnickname() +
				" :Welcome to the Internet Relay Network " + it->getnickname() +
				"!" + it->getnickname() + "@" + "localhost" + CLRF;
			send(fd, reply.c_str(), reply.length(), 0);
		}
	}
	else if (split[0] == "PING")
	{
		reply = "PONG " + split[1] + CLRF;
		send(fd, reply.c_str(), reply.length(), 0);
	}
	else if (split[0] == "WHO")
	{
		/*
			352	RPL_WHOREPLY
				"<channel> <user> <host> <server> <nick>
				<H|G>[*][@|+] :<hopcount> <real name>"
		*/
	}
	else if (split[0] == "USERHOST")
	{
		/*
			302	RPL_USERHOST
			":[<reply>{<space><reply>}]"
			- Reply format used by USERHOST to list replies to
				the query list.  The reply string is composed as
				follows:

				<reply> ::= <nick>['*'] '=' <'+'|'-'><hostname>

				The '*' indicates whether the client has registered
				as an Operator.  The '-' or '+' characters represent
				whether the client has set an AWAY message or not
				respectively.
		*/
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
			std::cout << "Client " << fd << " sent " << _server->fds[fd].rdbuf;
			t_strvect	split = ft_splitstring(_server->fds[fd].rdbuf, CLRF);
			for (size_t i = 0; i < split.size() && !split[i].empty(); i++)
				ProcessMessage(fd, split[i], _server);
			_server->fds[fd].rdbuf.erase();
		}
	}
	else
	{
		close(fd);
		_server->fds.erase(fd);
		t_citer	it = ft_findclientfd(_server->clients.begin(), _server->clients.end(), fd);
		if (it != _server->clients.end())
			it->Disconnect();
		std::cout << "Client " << fd << " disconnected" << std::endl;
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
