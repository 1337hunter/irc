/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/06 13:14:43 by salec             #+#    #+#             */
/*   Updated: 2020/10/27 18:31:07 by salec            ###   ########.fr       */
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
	int							sock;
	std::map<int, t_fd>			fds;
	t_cmdmap					command;
	std::string					pass;
	std::string					server;	// placeholder for some other server
	std::vector<Client>			clients;
	fd_set						fdset_read;
	static std::string const	clrf;
};

//typedef void (*commands_t)(int fd, const std::string &split, IRCserv *_server);

void		CreateSock(IRCserv *_server);
void		AcceptConnect(IRCserv *_server);
void		RecieveMessage(int fd, IRCserv *_server);
void		ProcessMessage(int fd, std::string const &msg, IRCserv *_server);
void		RunServer(IRCserv *_server);
t_citer		ft_findclientfd(t_citer const &begin, t_citer const &end, int fd);
t_citer		ft_findnick(t_citer const &begin, t_citer const &end, std::string const &nick);

#endif
