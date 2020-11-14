/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/24 12:11:19 by salec             #+#    #+#             */
/*   Updated: 2020/11/14 21:36:14 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

Client::Client() : fd(-1), _isConnected(false), _isRegistred(false),
	_isOperator(false), USER(false), NICK(false)
{
}

Client::Client(std::string const &nickname, int fd):
	nickname(nickname), fd(fd), _isConnected(true), _isRegistred(false),
	_isOperator(false), USER(false), NICK(true)
{
}

Client::Client(std::string const &username, std::string const &realname, int fd):
username(username), realname(realname), fd(fd), _isConnected(true), _isRegistred(false),
    _isOperator(false), USER(true), NICK(false)
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
	this->_isRegistred = other._isRegistred;
	this->_isOperator = other._isOperator;
	this->USER = other.USER;
	this->NICK = other.NICK;
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

bool	Client::Register(std::string const &user, std::string const &real)
{
	if (this->_isRegistred ||
		user.length() < 1 || user.length() > 10 ||
		real.length() < 1)
		return (false);
	this->username = user;
	this->realname = real;
	if (NICK)
		this->_isRegistred = true;
	USER = true;
	return (true);
}

bool	Client::Register(std::string const &nick)
{
	nickname = nick;
	if (USER)
		_isRegistred = true;
	NICK = true;
	return true;
}

void				Client::Reconnect(int fd)
{
	this->fd = fd;
	this->_isConnected = true;
}

void				Client::Disconnect(void)
{
	this->fd = -1;
	this->hostname = "";
	this->_isConnected = false;
	this->_isRegistred = false;
	this->_isOperator = false;
}

void	Client::ChangeNick(std::string const &what)
{
	this->nickname = what;
}

bool	Client::isRegistred(void)
{
	return _isRegistred;
}

void	Client::setFD(int what)
{
	fd = what;
}

bool	Client::getUSER(void)
{
	return USER;
}

bool	Client::getNICK(void)
{
	return NICK;
}

void	Client::setOPER(void)
{
	_isOperator = true;
}

bool Client::isOperator(void)
{
	return _isOperator;
}
