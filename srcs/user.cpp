/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 17:47:55 by salec             #+#    #+#             */
/*   Updated: 2020/10/22 18:22:08 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "user.hpp"

User::User()
{
}

User::User(std::string const &login, std::string const &nick,
	std::string const &real) : login(login), nick(nick), real(real)
{
}

User::~User()
{
}

User::User(User const &other)
{
	*this = other;
}

User	&User::operator=(User const &other)
{
	this->login = other.login;
	this->nick = other.nick;
	this->real = other.real;
	return (*this);
}
