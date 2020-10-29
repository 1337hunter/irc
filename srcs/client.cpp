/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/24 12:11:19 by salec             #+#    #+#             */
/*   Updated: 2020/10/29 21:05:12 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

Client::Client() : fd(-1), _isConnected(false), _isOperator(false)
{
}

Client::Client(std::string const &nickname, int fd) :
	nickname(nickname), fd(fd), _isConnected(true), _isOperator(false)
{
}

Client::~Client()
{
}

Client::Client(Client const &other)
{
	*this = other;
}

Client				&Client::operator=(Client const &other)
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

bool				Client::isConnected(void)
{
	return (this->_isConnected);
}

int const 			&Client::getFD(void)
{
	return (this->fd);
}

std::string const	&Client::getnickname(void)
{
	return (this->nickname);
}

std::string const	&Client::getusername(void)
{
	return (this->username);
}

std::string const	&Client::getrealname(void)
{
	return (this->realname);
}

bool				Client::Register(std::string const &user,
							std::string const &real)
{
	if (real[0] != ':' || real.size() < 2 || real.size() > 10 ||
		this->username == user || this->realname == real.substr(1))
		return (false);
	this->username = user;
	this->realname = real.substr(1);
	return (true);
}

void				Client::Disconnect(void)
{
	this->fd = -1;
	this->hostname = "";
	this->_isConnected = false;
}

void	Client::ChangeNick(std::string what)
{
	this->nickname = what;
}
