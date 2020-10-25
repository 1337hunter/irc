/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 16:44:15 by salec             #+#    #+#             */
/*   Updated: 2020/10/25 21:35:11 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

// does nothing (can open config maybe later???)
IRCserv::IRCserv() : sock(-1)
{
}

// handles <port>
IRCserv::IRCserv(std::string const &port) : sock(-1)
{
	if (port.find_first_not_of("0123456789") != std::string::npos ||
		port.length() < 1 || port.length() > 5)
		std::cerr << "Error: bad port number" << std::endl;
	else
	{
		this->port = stoi(port);
		if (this->port < 1 || this->port > 65535)
			std::cerr << "Error: bad port number" << std::endl;
		else
			this->RunServer();
	}
}

// handles <port> <password> or [host:port_network:password_network] <port>
IRCserv::IRCserv(std::string const &port, std::string const &pass) : sock(-1)
{
	if (port.find_first_not_of("0123456789") == std::string::npos)
	{
		if (port.length() < 1 || port.length() > 5)
			std::cerr << "Error: bad port number" << std::endl;
		else
		{
			this->pass = pass;
			this->port = stoi(port);
			if (this->port < 1 || this->port > 65535)
				std::cerr << "Error: bad port number" << std::endl;
			else
				this->RunServer();
		}
	}
	else
	{
		this->server = port;
		if (pass.find_first_not_of("0123456789") != std::string::npos ||
			pass.length() < 1 || pass.length() > 5)
			std::cerr << "Error: bad port number" << std::endl;
		else
		{
			this->port = stoi(pass);
			if (this->port < 1 || this->port > 65535)
				std::cerr << "Error: bad port number" << std::endl;
			else
				this->RunServer();
		}
	}
}

// handles [host:port_network:password_network] <port> <password>
IRCserv::IRCserv(std::string const &server, std::string const &port,
					std::string const &pass) : sock(-1)
{
	this->server = server;
	this->pass = pass;
	if (port.find_first_not_of("0123456789") != std::string::npos ||
			port.length() < 1 || port.length() > 5)
			std::cerr << "Error: bad port number" << std::endl;
	else
	{
		this->port = stoi(pass);
		if (this->port < 1 || this->port > 65535)
			std::cerr << "Error: bad port number" << std::endl;
		else
			this->RunServer();
	}
}

IRCserv::~IRCserv()
{
	// delete and close everything
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
	sockin.sin_addr.s_addr = INADDR_ANY;
	sockin.sin_port = htons(port);
	if (bind(this->sock, (t_sockaddr*)&sockin, sizeof(sockin)) < 0)
		error_exit("bind error (probably already binded)");
	if (listen(this->sock, 42) < 0)
		error_exit("listen error");
	std::cout << "Server created on sock " << this->sock << std::endl;
}

void		IRCserv::AcceptConnect(void)
{
	this->clients.push_back(Client(this->sock));
}

void		IRCserv::RunServer(void)
{
	CreateSock();
	while (1)
	{
		FD_ZERO(&(this->fdset_read));
		FD_SET(this->sock, &(this->fdset_read));
		int lastfd = this->sock;
		for (size_t i = 0; i < this->clients.size(); i++)
		{
			if (this->clients[i].isConnected())
			{
				FD_SET(this->clients[i].getFD(), &(this->fdset_read));
				lastfd = std::max(lastfd, this->clients[i].getFD());
			}
		}
		int readyfds = select(lastfd + 1, &(this->fdset_read), NULL, NULL, NULL);
		if (FD_ISSET(this->sock, &(this->fdset_read)))
		{
			AcceptConnect();
			readyfds--;
		}
		for (size_t i = 0; readyfds > 0 && i < this->clients.size(); i++)
		{
			if (this->clients[i].isConnected() &&
				FD_ISSET(this->clients[i].getFD(), &(this->fdset_read)))
			{
				this->clients[i].Recieve();
				readyfds--;
			}
		}
	}
}
