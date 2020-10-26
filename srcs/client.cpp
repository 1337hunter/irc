/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/24 12:11:19 by salec             #+#    #+#             */
/*   Updated: 2020/10/26 22:23:34 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

Client::Client() : fd(-1), _isConnected(false), _isOperator(false)
{
}

Client::Client(std::string const &nickname, int const &fd) :
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

void				Client::Register(std::string const &user,
							std::string const &real)
{
	this->username = user;
	this->realname = real;
}

void				Client::Disconnect(void)
{
	this->hostname = "";
	this->_isConnected = false;
}
