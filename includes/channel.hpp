/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/24 12:40:08 by salec             #+#    #+#             */
/*   Updated: 2020/12/07 20:27:02 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <string>
#include <vector>
#include "client.hpp"
#include <unordered_map>
#include <list>
//#include "tools.hpp"

/*	A channel is a named group of one or more clients which will all
	receive messages addressed to that channel.  The channel is created
	implicitly when the first client joins it, and the channel ceases to
	exist when the last client leaves it.  While channel exists, any
	client can reference the channel using the name of the channel.
	Channels names are strings (beginning with a '&' or '#' character) of
	length up to 200 characters.  Apart from the the requirement that the
	first character being either '&' or '#'; the only restriction on a
	channel name is that it may not contain any spaces (' '), a control G
	(^G or ASCII 7), or a comma (',' which is used as a list item
	separator by the protocol).	*/

class Client;

struct	channel_flags
{
	bool		_anonymous;
	bool		_invite_only;
	bool		_moderated;
	bool		_no_messages_outside;
	bool		_quiet;
	bool		_private;
	bool		_secret;
	bool		_reop;
	bool		_topic_settable_by_chop;
	std::string	_key;
	size_t		_limit_of_users;
	std::list<std::string>	_ban_mask;
	std::list<std::string>	_exception_mask;
	std::list<std::string>	_Invitation_mask;
	
	channel_flags(void);
	~channel_flags(void);
};

struct	client_flags
{
	bool	_Operator;
	bool	_operator;
	bool	_voice;

	client_flags(void);
	client_flags(bool Operator, bool oper, bool voice);
	client_flags(client_flags const &obj);
	client_flags(bool *mod);
	~client_flags(void) {}
	client_flags &operator=(client_flags const &obj);
};

class Channel {
private:
	std::string				_name;
	bool                    _blocked;
	time_t					_creation_time;
	int						_reop_delay;
	std::string				_safe_postfix;
	std::string				_topic;
	char					_type;
	channel_flags			_flags;
	std::unordered_map<Client*, client_flags>	_clients;
	Channel();
public:
	Channel(std::string const &name, Client *client);
	Channel(std::string const &name, Client *client, std::string const &modes);
	Channel(std::string const &name, std::string const &key, Client *client);
	~Channel();
	Channel(Channel const &other);
	Channel &operator=(Channel const &other);
	
	std::unordered_map<Client*, client_flags> &getclients(void);
	std::string const	&getname(void);
	std::string	const	&getkey(void);
	std::string const	&gettopic(void);
	channel_flags const	&getflags(void);
	Channel				*getptr(void);
	char				gettype(void);
	bool				isSecret(void);
	bool				isPrivate(void);
	void				add_client(Client *client);
	void				block(void);
	void				unblock(void);
	bool				isBlocked(void);
	bool				isOnChan(Client *client);
	bool				isInvited(Client *client);
	bool				isBanned(Client *client);

	void				settype(char type);
	void				settopic(std::string const &topic);
};

#endif
