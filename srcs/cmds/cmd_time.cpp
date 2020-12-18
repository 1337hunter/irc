/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_time.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/29 18:23:37 by salec             #+#    #+#             */
/*   Updated: 2020/12/18 18:22:51 by salec            ###   ########.fr       */
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
	std::string	nick = getnicktoreply(fd, split, serv);
	if (nick.empty())
	{
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
			ERR_NOTREGISTERED, "", "", "You have not registered");
		return ;
	}

	if (serv->fds[fd].type != FD_SERVER && (split.size() < 2 ||
		(split.size() > 1 && getservernamebymask(serv, split[1]) == serv->servername)))
	{
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername, RPL_TIME,
			nick, serv->servername, ft_timetostring(ft_getcurrenttime()));
	}
	else if (serv->fds[fd].type != FD_SERVER && split.size() > 1)
	{
		int	servfd = getserverfdbymask(serv, split[1]);
		if (servfd > 0 && !nick.empty())
			serv->fds[servfd].wrbuf += ":" + nick + " TIME " + split[1] + CRLF;
		else
			serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
				ERR_NOSUCHSERVER, nick, split[1], "No such server");
	}
	else if (split.size() >= 3)	// from another server: reply or forward
	{
		nick = split[0].substr(1);
		if (getservernamebymask(serv, split[2]) == serv->servername)
		{
			serv->fds[fd].wrbuf += ft_buildmsg(serv->servername, RPL_TIME,
				nick, serv->servername, ft_timetostring(ft_getcurrenttime()));
		}
		else
		{
			int	servfd = getserverfdbymask(serv, split[2]);
			if (servfd > 0)
				serv->fds[servfd].wrbuf += split[0] + " TIME " + split[2] + CRLF;
			else
				serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
					ERR_NOSUCHSERVER, nick, split[1], "No such server");
		}
	}
}
