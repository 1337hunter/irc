/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_oper.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbright <gbright@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/10 14:09:05 by gbright           #+#    #+#             */
/*   Updated: 2020/11/10 17:56:05 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

void    cmd_oper(int fd, const t_strvect &split, IRCserv *_server)
{
	std::vector<t_oper>::iterator	b = _server->oper.begin();
	std::vector<t_oper>::iterator	e = _server->oper.end();
	std::string	reply = _server->fds[fd].wrbuf = ":" + _server->hostname + " ";
	if (split.size() < 3)
	{
		_server->fds[fd].wrbuf = ":" + _server->hostname + " ";
		_server->fds[fd].wrbuf += ERR_NEEDMOREPARAMS;
		_server->fds[fd].wrbuf += " OPER :Not enough parameters";
		_server->fds[fd].wrbuf += CLRF;
		return ;
	}
	while (b != e)
	{
		if (b->name == split[1])
			break ;
		b++;
	}
	if (b == e)
	{
		_server->fds[fd].wrbuf = ":" + _server->hostname + " ";
		_server->fds[fd].wrbuf += ERR_NOOPERHOST;
		_server->fds[fd].wrbuf += " :No O-lines for your host";
		_server->fds[fd].wrbuf += CLRF;
		return ;
	}
	if (b->pass != split[2])
	{
		_server->fds[fd].wrbuf = ":" + _server->hostname + " ";
		_server->fds[fd].wrbuf += ERR_PASSWDMISMATCH;
		_server->fds[fd].wrbuf += " :Password incorrect";
		_server->fds[fd].wrbuf += CLRF;
		return ;
	}
	_server->fds[fd].type = FD_OPER;
	_server->fds[fd].wrbuf = ":" + _server->hostname + " ";
	_server->fds[fd].wrbuf += RPL_YOUREOPER;
	_server->fds[fd].wrbuf += " :You are now an IRC operator";
	_server->fds[fd].wrbuf += CLRF;
	return ;
}
