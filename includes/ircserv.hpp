/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/06 13:14:43 by salec             #+#    #+#             */
/*   Updated: 2020/10/27 14:36:43 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_HPP
#define IRCSERV_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
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

typedef struct			s_fd
{
	int					type;
	std::string			rdbuf;
}						t_fd;

struct IRCserv {
	int					port;
	int					sock;
	std::map<int, t_fd>	fds;
	std::string			pass;
	std::string			server;		// placeholder for some other server
	std::vector<Client>	clients;
	std::string	const	clrf = CLRF;
	fd_set				fdset_read;
};

void	CreateSock(IRCserv *_server);
void	AcceptConnect(IRCserv *_server);
void	RecieveMessage(int fd, IRCserv *_server);
void	ProcessMessage(int fd, std::string const &msg, IRCserv *_server);
void	RunServer(IRCserv *_server);

#endif
