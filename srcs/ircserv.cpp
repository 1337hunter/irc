/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 16:44:15 by salec             #+#    #+#             */
/*   Updated: 2020/10/26 17:10:59 by gbright          ###   ########.fr       */
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
	sockin.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1"); can changee ip
	// to create another local server without the b8s or containers just with ip variable seted in config file
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
