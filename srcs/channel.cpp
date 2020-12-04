/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/24 12:43:52 by salec             #+#    #+#             */
/*   Updated: 2020/12/04 16:03:14 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"
#include "tools.hpp"

/*	Channel::Channel() : chop()	{}	*/

channel_flags::channel_flags(void) : _anonymous(false), _invite_only(false),
	_moderated(false), _no_messages_outside(false), _quiet(false), _private(false),
	_secret(false), _reop(false), _topic_settable_by_chop(false), _key(""),
	_limit_of_users(1024), _ban_mask(""), _exception_mask(""), _Invitation_mask("") {}

channel_flags::~channel_flags(void) {}

client_flags::client_flags(bool Operator, bool oper, bool voice) :
	_Operator(Operator), _operator(oper), _voice(voice) {}

client_flags::client_flags(client_flags const &obj)
{
	_Operator = obj._Operator;
	_operator = obj._operator;
	_voice = obj._voice;
}

client_flags	&client_flags::operator=(client_flags const &obj)
{
	_Operator = obj._Operator;
	_operator = obj._operator;
	_voice = obj._voice;
	return *this;
}

client_flags::client_flags(void) : _Operator(0), _operator(0), _voice(0) {}

Channel::Channel(std::string const &name, Client *client) :
	_name(name)
{
	client->add_channel(this);
	_clients[client] = client_flags(1, 1, 0);
}

Channel::~Channel() {}

Channel::Channel(Channel const &other)
{
	*this = other;
}

Channel::Channel(std::string const &name, std::string const &key, Client *client) :
	_name(name)
{
	_flags._key = key;
	client->add_channel(this);
	_clients[client] = client_flags(1, 1, 0);
}

Channel	&Channel::operator=(Channel const &other)
{
	_name = other._name;
	_clients = other._clients;
	return (*this);
}

std::unordered_map<Client*, client_flags> &Channel::getclients(void)
{
	return _clients;
}

std::string	const &Channel::getname(void)
{
	return _name;
}

std::string	const &Channel::getkey(void)
{
	return _flags._key;
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
	client->add_channel(this);
	_clients[client];
}

bool		Channel::isSecret(void)
{
	return _flags._secret;
}

bool		Channel::isPrivate(void)
{
	return _flags._private;
}

std::string	const &Channel::gettopic(void)
{
	return _topic;
}

void		Channel::settopic(std::string const &topic)
{
	_topic = topic;
}

channel_flags const	&Channel::getchanflags(void)
{
	return _flags;
}
