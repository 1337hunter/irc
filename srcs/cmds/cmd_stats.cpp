/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_stats.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/17 20:31:42 by salec             #+#    #+#             */
/*   Updated: 2020/12/25 20:26:09 by salec            ###   ########.fr       */
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
	<query> parameter is omitted, only the end of stats reply is sent back.

	A query may be given for any single letter which is only checked by
	the destination server and is otherwise passed on by intermediate
	servers, ignored and unaltered.
	Wildcards are allowed in the <target> parameter.

	Except for the ones below, the list of valid queries is
	implementation dependent.  The standard queries below SHOULD be
	supported by the server:
		l - returns a list of the server's connections, showing how long
			each connection has been established and the traffic over
			that connection in Kbytes and messages for each direction;
		m - returns the usage count for each of commands supported by the
			server; commands for which the usage count is zero MAY be omitted;
		o - returns a list of configured privileged users, operators;
		u - returns a string showing how long the server has been up.
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

std::string	getserverupstr(IRCserv *serv)
{
	time_t		elapsed = ft_getcurrenttime() - serv->dtstarted;
	time_t		sec, min, hrs;
	std::string	res = "Server Up " + TOSTRING(elapsed / 86400) + " days ";
	elapsed %= 86400;

	sec = elapsed % 60;
	elapsed	/= 60;
	min = elapsed % 60;
	elapsed	/= 60;
	hrs = elapsed % 24;

	res += TOSTRING(hrs) + ":";
	if (min < 10)
		res += "0";
	res += TOSTRING(min) + ":";
	if (sec < 10)
		res += "0";
	res += TOSTRING(sec);
	return (res);
}

typedef	std::MAP<std::string, Command>::iterator	t_cmdit;
typedef	std::MAP<int, t_fd>::iterator				t_fdrit;
typedef	std::vector<t_oper>::iterator						t_opit;

std::string	reply_stats(IRCserv *serv, std::string const &target,
	std::string const &modearg)
{
	std::string	reply = "";
	std::string	mode = "*";
	if (modearg.size() > 0)
		mode = ft_strtolower(modearg.substr(0, 1));

	if (mode == "l")
	{
		//	for now it's reversed
		for (t_fdrit it = serv->fds.begin(); it != serv->fds.end(); it++)
			reply += ft_buildmsg(serv->servername, RPL_STATSLINKINFO, target,
				it->second.linkname + " " +
				TOSTRING(it->second.wrbuf.size()) + " " +
				TOSTRING(it->second.sentmsgs) + " " +
				TOSTRING(it->second.sentbytes/* / 1024*/) + " " +
				TOSTRING(it->second.recvmsgs) + " " +
				TOSTRING(it->second.recvbytes/* / 1024*/) + " " +
				TOSTRING(ft_getcurrenttime() - it->second.dtopened), "");
	}
	else if (mode == "m")
	{
		for (t_cmdit it = serv->cmds.begin(); it != serv->cmds.end(); it++)
			if (it->second.used())
				reply += ft_buildmsg(serv->servername, RPL_STATSCOMMANDS,
					target, it->first + " " +
					TOSTRING(it->second.getcount()) + " " +
					TOSTRING(it->second.getbytes()) + " " +
					TOSTRING(it->second.getrcount()), "");
	}
	else if (mode == "o")
	{
		for (t_opit it = serv->oper.begin(); it != serv->oper.end(); it++)
			reply += ft_buildmsg(serv->servername, RPL_STATSOLINE, target,
				std::string("O") + " " + it->hostmask + " * " + it->name, "");
	}
	else if (mode == "u")
	{
		reply = ft_buildmsg(serv->servername, RPL_STATSUPTIME, target, "",
			getserverupstr(serv));
	}
	reply += ft_buildmsg(serv->servername, RPL_ENDOFSTATS, target, mode,
		"End of STATS report");
	return (reply);
}

void		cmd_stats(int fd, const t_strvect &split, IRCserv *serv)
{
	t_fd		&fdref = serv->fds[fd];
	std::string	nick = getnicktoreply(fd, split, serv);
	if (nick.empty())
	{
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NOTREGISTERED, "", "", "You have not registered");
		return ;
	}

	if (fdref.type != FD_SERVER && (split.size() < 3 ||
		(split.size() > 2 && getservernamebymask(serv, split[2]) == serv->servername)))
	{
		if (split.size() < 2)
			fdref.wrbuf += reply_stats(serv, nick, "*");
		else
			fdref.wrbuf += reply_stats(serv, nick, split[1]);
	}
	else if (fdref.type != FD_SERVER && split.size() > 2)
	{
		int	servfd = getserverfdbymask(serv, split[2]);
		if (servfd > 0)
			serv->fds[servfd].wrbuf += ":" + nick + " STATS " +
				split[1][0] + " " + split[2] + CRLF;
		else
			fdref.wrbuf += ft_buildmsg(serv->servername,
				ERR_NOSUCHSERVER, nick, split[2], "No such server");
	}
	else if (split.size() >= 4)	// from another server: reply or forward
	{
		if (getservernamebymask(serv, split[3]) == serv->servername)
			fdref.wrbuf += reply_stats(serv, nick, split[2]);
		else
		{
			int	servfd = getserverfdbymask(serv, split[3]);
			if (servfd > 0)
				serv->fds[servfd].wrbuf += split[0] + " STATS " +
					split[2][0] + " " + split[3] + CRLF;
			else
				fdref.wrbuf += ft_buildmsg(serv->servername,
					ERR_NOSUCHSERVER, nick, split[3], "No such server");
		}
	}
}
