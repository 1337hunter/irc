/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/24 12:43:52 by salec             #+#    #+#             */
/*   Updated: 2020/10/24 12:53:13 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"

/*	Channel::Channel() : chop()	{}	*/

Channel::Channel(std::string const &name, Client &creator) :
	name(name), chop(creator)
{
}

Channel::~Channel()
{
}

Channel::Channel(Channel const &other) : chop(other.chop)
{
	*this = other;
}

Channel	&Channel::operator=(Channel const &other)
{
	this->name = other.name;
	this->joined = other.joined;
	this->chop = other.chop;
	return (*this);
}
