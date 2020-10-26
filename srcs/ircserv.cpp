/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 16:44:15 by salec             #+#    #+#             */
/*   Updated: 2020/10/26 23:28:20 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

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
	this->fds[this->sock] = FD_SERVER;
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
	this->fds[fd] = FD_CLIENT;
}

void		IRCserv::ProcessMessage(int const &fd, std::string const &msg)
{
	std::vector<std::string>		split = ft_splitstring(msg, " ");
	std::string						reply;
	std::vector<Client>::iterator	it;

	if (split[0] == "NICK")
	{
		std::string	nick = split[1];
		if (std::find_if(this->clients.begin(), this->clients.end(),
			[&nick](Client c){ return (c.getnickname() == nick); })
			== this->clients.end())
			this->clients.push_back(Client(split[1], fd));
		else
		{
			reply = ":localhost ";
			reply += ERR_NICKNAMEINUSE;
			reply += " " + nick + " :Nickname is already in use" + CLRF;
			send(fd, reply.c_str(), reply.length(), 0);
		}
	}
	else if (split[0] == "USER")
	{
		it = std::find_if(this->clients.begin(), this->clients.end(),
			[&fd](Client c){ return (c.getFD() == fd); });
		if (it != this->clients.end())
			it->Register(split[1], split[4]);
		reply = ":localhost ";
		reply += RPL_WELCOME;
		reply += " " + it->getnickname() +
			" :Welcome to the Internet Relay Network " + it->getnickname() +
			"!" + it->getnickname() + "@" + "localhost" + CLRF;
		send(fd, reply.c_str(), reply.length(), 0);
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
	ssize_t						r;
	char						buf_read[BUF_SIZE + 1];
	std::vector<std::string>	split;

	if ((r = recv(fd, buf_read, BUF_SIZE, 0)) >= 0)
		buf_read[r] = 0;
	if (r > 0)
	{
		std::cout << "Client " << fd << " sent " << buf_read;
		split = ft_splitstring(buf_read, CLRF);
		for (size_t i = 0; i < split.size() && !split[i].empty(); i++)
			this->ProcessMessage(fd, split[i]);
	}
	else
	{
		close(fd);
		this->fds[fd] = FD_FREE;
		std::vector<Client>::iterator it =
			std::find_if(this->clients.begin(), this->clients.end(),
			[&fd](Client s){ return (s.getFD() == fd); });	// lambda expr
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
			if (this->fds[i] != FD_FREE)
			{
				FD_SET(i, &(this->fdset_read));
				lastfd = std::max(lastfd, i);
			}
		}
		int readyfds = select(lastfd + 1, &(this->fdset_read), NULL, NULL, NULL);
		for (int i = 0; readyfds > 0 && i < FD_MAX; i++)
		{
			if (this->fds[i] != FD_FREE && FD_ISSET(i, &(this->fdset_read)))
			{
				if (this->fds[i] == FD_SERVER)
					this->AcceptConnect();
				else if (this->fds[i] == FD_CLIENT)
					this->RecieveMessage(i);
				readyfds--;
			}
		}
	}
}
