/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/24 12:43:52 by salec             #+#    #+#             */
/*   Updated: 2020/12/01 23:04:15 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"
#include "tools.hpp"

/*	Channel::Channel() : chop()	{}	*/

Channel::Channel(std::string const &name, Client &creator) :
	_name(::ft_strtoupper(name)), _chop(creator)
{
}

Channel::~Channel()
{
}

Channel::Channel(Channel const &other) : _chop(other._chop)
{
	*this = other;
}

Channel	&Channel::operator=(Channel const &other)
{
	_name = other._name;
	_clients = other._clients;
	_chop = other._chop;
	return (*this);
}


std::string	const &Channel::getname(void)
{
	return _name;
}

std::string	const &Channel::getkey(void)
{
	return _key;
}

char		Channel::gettype(void)
{
	return _type;
}

void		Channel::settype(char type)
{
	_type = type;
}

void		Channel::add_client(Client *client)
{
	client = 0;
}
