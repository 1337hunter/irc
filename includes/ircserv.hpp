/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/06 13:14:43 by salec             #+#    #+#             */
/*   Updated: 2020/10/22 17:43:23 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_HPP
#define IRCSERV_HPP

#include <iostream>
#include <string>

class IRCserv {
private:
	int				port;
	std::string		pass;
	std::string		server;
	IRCserv();
public:
	// handles <port>
	IRCserv(std::string const &port);
	// handles <port> <password> or [host:port_network:password_network] <port>
	IRCserv(std::string const &port, std::string const &pass);
	// handles [host:port_network:password_network] <port> <password>
	IRCserv(std::string const &server, std::string const &port,
				std::string const &pass);
	~IRCserv();
	IRCserv(IRCserv const &other);
	IRCserv &operator=(IRCserv const &other);

	void			RunServer(void);
};

#endif
