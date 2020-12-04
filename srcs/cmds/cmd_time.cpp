/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_time.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/29 18:23:37 by salec             #+#    #+#             */
/*   Updated: 2020/12/04 18:43:24 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

/*
	Command: TIME
	Parameters: [ <target> ]

	The time command is used to query local time from the specified
	server. If the <target> parameter is not given, the server receiving
	the command must reply to the query.
	Wildcards are allowed in the <target> parameter.

	Numeric Replies:
		ERR_NOSUCHSERVER
		RPL_TIME

	Examples:
	TIME tolsun.oulu.fi		; check the time on the server "tolson.oulu.fi"
*/

void	cmd_time(int fd, const t_strvect &split, IRCserv *serv)
{
	t_citer		cit = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	std::string	target = "";
	if (cit != serv->clients.end())
		target = cit->getnickname();

	if (split.size() > 1)
		std::cerr << "info does not support other servers yet" << std::endl;

	time_t	rawtime = ft_getcurrenttime();
	serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
		RPL_TIME, target, serv->servername, ft_timetostring(rawtime));
}

