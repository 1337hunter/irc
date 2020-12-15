/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_motd.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/21 17:32:05 by salec             #+#    #+#             */
/*   Updated: 2020/12/15 16:45:01 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"

/*
	Command: MOTD
	Parameters: [ <target> ]

	The MOTD command is used to get the "Message Of The Day" of the given
	server, or current server if <target> is omitted.
	Wildcards are allowed in the <target> parameter.

	Numeric Replies:
		RPL_MOTDSTART
		RPL_MOTD
		RPL_ENDOFMOTD
		ERR_NOMOTD
*/

void	cmd_motd(int fd, const t_strvect &split, IRCserv *serv)
{
	std::string	nick;
	t_citer		it;

	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (it != serv->clients.end())
		nick = it->getnickname();

	if (serv->fds[fd].type != FD_SERVER && (split.size() < 2 ||
		(split.size() >= 2 && getservernamebymask(serv, split[1]) == serv->servername)))
		serv->fds[fd].wrbuf += reply_motd(serv, nick);
	else if (serv->fds[fd].type != FD_SERVER)
	{
		int	servfd;
		if ((servfd = getserverfdbymask(serv, split[1])) > 0 && !nick.empty())
			serv->fds[servfd].wrbuf += ":" + nick + " MOTD " + split[1] + CRLF;
	}
	else	// from another server: forward this further or reply
	{
		if (split.size() >= 3 && getservernamebymask(serv, split[2]) == serv->servername)
		{
			nick = split[0].substr(1);
			serv->fds[fd].wrbuf += reply_motd(serv, nick);
		}
		else
		{
			int	servfd;
			if (split.size() > 2 && (servfd = getserverfdbymask(serv, split[2])) > 0)
				serv->fds[servfd].wrbuf += split[0] + " MOTD " + split[2] + CRLF;
		}
	}
}
