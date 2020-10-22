/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 16:44:15 by salec             #+#    #+#             */
/*   Updated: 2020/10/22 17:47:15 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

// does nothing (can open config maybe later???)
IRCserv::IRCserv()
{
}

// handles <port>
IRCserv::IRCserv(std::string const &port)
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
IRCserv::IRCserv(std::string const &port, std::string const &pass)
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
					std::string const &pass)
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
	this->pass = other.pass;
	this->server = other.server;
	return (*this);
}

void		IRCserv::RunServer(void)
{
	// run it lol
}
