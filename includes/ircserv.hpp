/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/06 13:14:43 by salec             #+#    #+#             */
/*   Updated: 2020/10/25 19:11:22 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_HPP
#define IRCSERV_HPP

#include <iostream>
#include <string>
#include <vector>
// fcntl, select, socket, inet structs, inet
#include <fcntl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
// read, write
#include <unistd.h>

#include "error_handle.hpp"
#include "common_defines.hpp"
#include "client.hpp"

class IRCserv {
private:
	int					port;
	int					sock;
	std::string			pass;
	std::string			server;		// placeholder for some other server
	std::vector<Client>	clients;
	fd_set				fdset_read;
	IRCserv();

	void				CreateSock(void);
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

	void				AcceptConnect(void);
	void				RunServer(void);
};

#endif
