/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/24 12:11:19 by salec             #+#    #+#             */
/*   Updated: 2020/10/26 17:32:16 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

Client::Client() : fd(-1), _isConnected(false), _isOperator(false)
{
}

Client::Client(int const &sock)
{
	t_sockaddr_in	csin;
	socklen_t		csin_len;

	csin_len = sizeof(csin);
	if ((this->fd = accept(sock, (t_sockaddr*)&csin, &csin_len)) < 0)
		error_exit("accept error");
	if (fcntl(this->fd, F_SETFL, O_NONBLOCK) < 0)
		error_exit("fcntl error: failed to set nonblock fd");
	std::cout << "Client " << this->fd << " from " <<
		inet_ntoa(csin.sin_addr) << ":" << ntohs(csin.sin_port) <<
		" accepted" << std::endl;
	this->_isConnected = true;
}

Client::~Client()
{
}

Client::Client(Client const &other)
{
	*this = other;
}

Client		&Client::operator=(Client const &other)
{
	this->fd = other.fd;
	this->nickname = other.nickname;
	this->username = other.username;
	this->realname = other.realname;
	this->hostname = other.hostname;
	this->_isConnected = other._isConnected;
	this->_isOperator = other._isOperator;
	return (*this);
}

int const 	&Client::getFD(void)
{
	return (this->fd);
}

bool		Client::isConnected(void)
{
	return (this->_isConnected);
}

std::vector<std::string>	ft_splitstring(std::string msg, std::string delim)
{
	std::vector<std::string>	split;
	size_t						pos = 0;
	std::string					token;
	std::string					reply;

	while ((pos = msg.find(delim)) != std::string::npos)
	{
		split.push_back(msg.substr(0, pos));
		msg.erase(0, pos + delim.length());
	}
	split.push_back(msg);
	return (split);
}

void		Client::processMessage(std::string msg)
{
	std::vector<std::string>	split = ft_splitstring(msg, " ");
	std::string					reply;

	if (split[0] == "NICK")
	{
		this->nickname = split[1].substr(0, split[1].size());
		// do something if nickname is already taken
	}
	else if (split[0] == "USER")
	{
		this->username = split[1];
		this->realname = split[4].substr(1, split[4].size());
		reply = ":localhost 001 " + this->nickname +
			" :Welcome to localhost!" + CLRF;
		send(this->fd, reply.c_str(), reply.length(), 0);
	}
	else if (split[0] == "PING")
	{
		reply = "PONG " + split[1] + CLRF;
		send(this->fd, reply.c_str(), reply.length(), 0);
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

void		Client::Recieve(void)
{
	ssize_t		r;
	char		buf_read[BUF_SIZE + 1];

	r = recv(this->fd, buf_read, BUF_SIZE, 0);
	if (r >= 0)
		buf_read[r] = 0;
	if (r > 0 && std::string(buf_read).compare("disconnect\n"))
	{
		if (!std::string(buf_read).compare("exit\n"))
			exit(0);
		std::cout << "Client " << this->fd << " sent " << buf_read;
		if (buf_read[r - 1] != '\n')
			std::cout << std::endl;
		std::vector<std::string>	split = ft_splitstring(buf_read, CLRF);
		for (int i = 0; !split[i].empty(); i++)
			this->processMessage(split[i]);
	}
	else
	{
		close(this->fd);
/*		t_vectiter rmif = std::remove_if(e->clients.begin(), e->clients.end(),
			[cs](t_fd s){ return (s.fd == cs); });	// lambda expr
		e->clients.erase(rmif, e->clients.end());	// erase after rmif
*/		std::cout << "Client " << this->fd << " disconnected" << std::endl;
		this->_isConnected = false;
		this->fd = -1;
	}
}
