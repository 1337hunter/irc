/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/06 13:14:43 by salec             #+#    #+#             */
/*   Updated: 2020/10/28 11:22:22 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_HPP
#define IRCSERV_HPP

#include <iostream>
#include <exception>
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
// #include <unordered_map>

typedef std::vector<Client>::iterator		t_citer;

typedef struct		s_fd
{
	int				type;
	std::string		rdbuf;
}					t_fd;

struct IRCserv {
	typedef void (*t_command)(int fd, const t_strvect &split, IRCserv *_server);
	typedef std::map<std::string, t_command>	t_cmdmap;
	int							port;
	std::string					host;
	int							sock;
	std::map<int, t_fd>			fds;
	t_cmdmap					command;
	std::string					pass;
	std::vector<std::string>	connected_to;
	std::vector<std::string>	connect_from;
	std::vector<Client>			clients;
	fd_set						fdset_read;
	static std::string const	clrf;
};

void		CreateSock(IRCserv *_server);
void		AcceptConnect(IRCserv *_server);
void		RecieveMessage(int fd, IRCserv *_server);
void		ProcessMessage(int fd, std::string const &msg, IRCserv *_server);
void		RunServer(IRCserv *_server);
t_citer		ft_findclientfd(t_citer const &begin, t_citer const &end, int fd);
t_citer		ft_findnick(t_citer const &begin, t_citer const &end, std::string const &nick);

#endif
