/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/06 13:14:43 by salec             #+#    #+#             */
/*   Updated: 2020/10/26 21:45:56 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_HPP
#define IRCSERV_HPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
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
#include "reply_codes.hpp"
#include "error_codes.hpp"
#include "tools.hpp"
#include "client.hpp"

struct IRCserv {
	int					port;
	int					sock;
	int					*fds;
	std::string			pass;
	std::string			server;		// placeholder for some other server
	std::vector<Client>	clients;
	fd_set				fdset_read;

	IRCserv();
	~IRCserv();
	IRCserv(IRCserv const &other);
	IRCserv &operator=(IRCserv const &other);
	void				CreateSock(void);
	void				AcceptConnect(void);
	void				RecieveMessage(int const &fd);
	void				ProcessMessage(int const &fd, std::string const &msg);
	void				RunServer(void);
};

#endif
