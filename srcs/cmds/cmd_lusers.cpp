/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_lusers.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/19 15:04:36 by salec             #+#    #+#             */
/*   Updated: 2020/12/19 15:10:34 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

/*
	Command: LUSERS
	Parameters: [ <mask> [ <target> ] ]

	The LUSERS command is used to get statistics about the size of the
	IRC network.  If no parameter is given, the reply will be about the
	whole net.  If a <mask> is specified, then the reply will only
	concern the part of the network formed by the servers matching the
	mask.  Finally, if the <target> parameter is specified, the request
	is forwarded to that server which will generate the reply.

	Wildcards are allowed in the <target> parameter.

	Numeric Replies:
		RPL_LUSERCLIENT
		RPL_LUSEROP
		RPL_LUSERUNKOWN
		RPL_LUSERCHANNELS
		RPL_LUSERME
		ERR_NOSUCHSERVER
*/

/*
	In particular the server SHALL send the current user/service/server
	count (as per the LUSER reply) and finally the MOTD (if any, as per
	the MOTD reply. Welcome change needed (RFC2813)

	reply_lusers:
:example.irc.org 251 arcticfox :There are 1 users and 0 services on 1 servers
:example.irc.org 254 arcticfox 14 :channels formed
:example.irc.org 255 arcticfox :I have 1 users, 0 services and 0 servers
:example.irc.org 265 arcticfox 1 1 :Current local users 1, max 1
:example.irc.org 266 arcticfox 1 1 :Current global users 1, max 1
*/

void	cmd_lusers(int fd, const t_strvect &split, IRCserv *serv)
{
	std::string	nick = getnicktoreply(fd, split, serv);
	if (nick.empty())
	{
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
			ERR_NOTREGISTERED, "", "", "You have not registered");
		return ;
	}


	// placeholder from TIME below. do LUSERS


	if (serv->fds[fd].type != FD_SERVER && (split.size() < 2 ||
		(split.size() > 1 && getservernamebymask(serv, split[1]) == serv->servername)))
	{
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername, RPL_TIME,
			nick, serv->servername, ft_timetostring(ft_getcurrenttime()));
	}
	else if (serv->fds[fd].type != FD_SERVER && split.size() > 1)
	{
		int	servfd = getserverfdbymask(serv, split[1]);
		if (servfd > 0)
			serv->fds[servfd].wrbuf += ":" + nick + " LINKS " + split[1] + CRLF;
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
				serv->fds[servfd].wrbuf += split[0] + " LINKS " + split[2] + CRLF;
			else
				serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
					ERR_NOSUCHSERVER, nick, split[1], "No such server");
		}
	}
}
