/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_stats.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/17 20:31:42 by salec             #+#    #+#             */
/*   Updated: 2020/12/18 18:17:15 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

/*
	Command: STATS
	Parameters: [ <query> [ <target> ] ]

	The stats command is used to query statistics of certain server.  If
	<query> parameter is omitted, only the end of stats reply is sent
	back.

	A query may be given for any single letter which is only checked by
	the destination server and is otherwise passed on by intermediate
	servers, ignored and unaltered.
	Wildcards are allowed in the <target> parameter.

	Except for the ones below, the list of valid queries is
	implementation dependent.  The standard queries below SHOULD be
	supported by the server:
		l - returns a list of the server's connections, showing how
			long each connection has been established and the
			traffic over that connection in Kbytes and messages for
			each direction;
		m - returns the usage count for each of commands supported
			by the server; commands for which the usage count is
			zero MAY be omitted;
		o - returns a list of configured privileged users,
			operators;
		u - returns a string showing how long the server has been
			up.
	It is also RECOMMENDED that client and server access configuration be
	published this way.

	Numeric Replies:
		ERR_NOSUCHSERVER
		RPL_STATSLINKINFO
		RPL_STATSUPTIME
		RPL_STATSCOMMANDS
		RPL_STATSOLINE
		RPL_ENDOFSTATS

	Examples:
	STATS m		; Check the command usage for this server
*/

void	cmd_stats(int fd, const t_strvect &split, IRCserv *serv)
{
	std::string	nick;
	t_citer		it;

	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (it != serv->clients.end())
		nick = it->getnickname();
	else if (split[0][0] == ':')
		nick = split[0].substr(1);
	else
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
			ERR_NOTREGISTERED, "", "", "You have not registered");


	// placeholder from TIME below. do STATS


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
