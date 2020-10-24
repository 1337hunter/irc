/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/24 12:11:19 by salec             #+#    #+#             */
/*   Updated: 2020/10/24 12:17:02 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

Client::Client()
{
}

Client::Client(std::string const &login, std::string const &nick,
	std::string const &real) : login(login), nickname(nick), realname(real)
{
	// getting hostname maybe here
}

Client::~Client()
{
}

Client::Client(Client const &other)
{
	*this = other;
}

Client	&Client::operator=(Client const &other)
{
	this->login = other.login;
	this->nickname = other.nickname;
	this->realname = other.realname;
	this->hostname = other.hostname;
	this->isOperator = other.isOperator;
	return (*this);
}
