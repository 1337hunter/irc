/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 16:44:15 by salec             #+#    #+#             */
/*   Updated: 2020/10/27 09:31:31 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include <cstring>

typedef	std::vector<Client>::iterator	t_citer;
typedef	std::vector<std::string>		t_strvect;

// does nothing (can open config maybe later???)
IRCserv::IRCserv() : sock(-1)
{
}

IRCserv::~IRCserv()
{
	int	var = 1;
	setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &var, sizeof(int));
	close(this->sock);
	delete[] (this->fds);
	this->fds = NULL;
}

IRCserv::IRCserv(IRCserv const &other)
{
	*this = other;
}

IRCserv 	&IRCserv::operator=(IRCserv const &other)
{
	this->port = other.port;
	this->sock = other.sock;
	this->pass = other.pass;
	this->server = other.server;
	this->clients = other.clients;
	this->fdset_read = other.fdset_read;
	return (*this);
}

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

void		IRCserv::CreateSock(void)
{
	t_sockaddr_in	sockin;
	t_protoent		*pe;

	if (!(pe = getprotobyname("tcp")))
		error_exit("getprotobyname error");
	if ((this->sock = socket(PF_INET, SOCK_STREAM, pe->p_proto)) < 0)
		error_exit("socket error");
	if (fcntl(this->sock, F_SETFL, O_NONBLOCK) < 0)
		error_exit("fcntl error: failed to set nonblock fd");
	sockin.sin_family = AF_INET;
	sockin.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1"); can changee ip
	// to create another local server without the b8s or containers just with ip variable seted in config file
	sockin.sin_port = htons(this->port);
	if (bind(this->sock, (t_sockaddr*)&sockin, sizeof(sockin)) < 0)
		error_exit("bind error (probably already binded)");
	if (listen(this->sock, 42) < 0)
		error_exit("listen error");
	this->fds[this->sock].type = FD_SERVER;
	std::cout << "Server created on sock " << this->sock << std::endl;
}

void		IRCserv::AcceptConnect(void)
{
	int				fd;
	t_sockaddr_in	csin;
	socklen_t		csin_len;

	csin_len = sizeof(csin);
	if ((fd = accept(sock, (t_sockaddr*)&csin, &csin_len)) < 0)
		error_exit("accept error");
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		error_exit("fcntl error: failed to set nonblock fd");
	std::cout << "Client " << fd << " from " <<
		inet_ntoa(csin.sin_addr) << ":" << ntohs(csin.sin_port) <<
		" accepted" << std::endl;
	this->fds[fd].type = FD_CLIENT;
	this->fds[fd].rdbuf.erase();
}

void		IRCserv::ProcessMessage(int const &fd, std::string const &msg)
{
	t_strvect		split = ft_splitstring(msg, " ");
	t_citer			it;
	std::string		reply;

	if (split[0] == "NICK")
	{
		if (ft_findnick(this->clients.begin(), this->clients.end(), split[1]) ==
			this->clients.end())
			this->clients.push_back(Client(split[1], fd));
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
		it = ft_findclientfd(this->clients.begin(), this->clients.end(), fd);
		if (it != this->clients.end())
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

void		IRCserv::RecieveMessage(int const &fd)
{
	ssize_t		r;
	char		buf_read[BUF_SIZE + 1];

	if ((r = recv(fd, buf_read, BUF_SIZE, 0)) >= 0)
		buf_read[r] = 0;
	if (r > 0)
	{
		this->fds[fd].rdbuf += buf_read;
		if (this->fds[fd].rdbuf.rfind(CLRF) + strlen(CLRF) ==
			this->fds[fd].rdbuf.length())
		{
			std::cout << "Client " << fd << " sent " << fds[fd].rdbuf;
			t_strvect	split = ft_splitstring(this->fds[fd].rdbuf, CLRF);
			for (size_t i = 0; i < split.size() && !split[i].empty(); i++)
				this->ProcessMessage(fd, split[i]);
			fds[fd].rdbuf.erase();
		}
	}
	else
	{
		close(fd);
		this->fds[fd].type = FD_FREE;
		this->fds[fd].rdbuf.erase();
		t_citer	it = ft_findclientfd(this->clients.begin(), this->clients.end(), fd);
		if (it != this->clients.end())
			it->Disconnect();
		std::cout << "Client " << fd << " disconnected" << std::endl;
	}
}

void		IRCserv::RunServer(void)
{
	CreateSock();
	while (1)
	{
		int	lastfd = 0;
		FD_ZERO(&(this->fdset_read));
		for (int i = 0; i < FD_MAX; i++)
		{
			if (this->fds[i].type != FD_FREE)
			{
				FD_SET(i, &(this->fdset_read));
				lastfd = std::max(lastfd, i);
			}
		}
		int readyfds = select(lastfd + 1, &(this->fdset_read), NULL, NULL, NULL);
		for (int i = 0; readyfds > 0 && i < FD_MAX; i++)
		{
			if (this->fds[i].type != FD_FREE && FD_ISSET(i, &(this->fdset_read)))
			{
				if (this->fds[i].type == FD_SERVER)
					this->AcceptConnect();
				else if (this->fds[i].type == FD_CLIENT)
					this->RecieveMessage(i);
				readyfds--;
			}
		}
	}
}
