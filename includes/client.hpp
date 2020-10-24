/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/24 12:10:42 by salec             #+#    #+#             */
/*   Updated: 2020/10/24 12:14:17 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <string>

/* A client is anything connecting to a server that is not another
   server.  Each client is distinguished from other clients by a unique
   nickname having a maximum length of nine (9) characters.
   See the protocol grammar rules for what may and may not be used in a
   nickname.  In addition to the nickname, all servers must have the
   following information about all clients: the real name of the host
   that the client is running on, the username of the client on that
   host, and the server to which the client is connected. */

class Client {
private:
	std::string		login;			// username
	std::string		nickname;
	std::string		realname;
	std::string		hostname;
	bool			isOperator;
	// Maybe server class object reference or pointer here
	Client();
public:
	Client(std::string const &login, std::string const &nick,
		std::string const &real);
	~Client();
	Client(Client const &other);
	Client &operator=(Client const &other);

	// getters-setters here
};

#endif
