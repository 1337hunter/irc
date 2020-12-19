/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_stats.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/17 20:31:42 by salec             #+#    #+#             */
/*   Updated: 2020/12/19 22:27:13 by salec            ###   ########.fr       */
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
	std::string	res = "Server Up " + std::to_string(elapsed / 86400) + " days ";
	elapsed %= 86400;

	sec = elapsed % 60;
	elapsed	/= 60;
	min = elapsed % 60;
	elapsed	/= 60;
	hrs = elapsed % 24;

	res += std::to_string(hrs) + ":";
	if (min < 10)
		res += "0";
	res += std::to_string(min) + ":";
	if (sec < 10)
		res += "0";
	res += std::to_string(sec);
	return (res);
}

typedef	std::map<int, t_fd>::reverse_iterator	t_fdrit;
typedef	std::vector<t_oper>::iterator			t_opit;

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
		for (t_fdrit it = serv->fds.rbegin(); it != serv->fds.rend(); it++)
			reply += ft_buildmsg(serv->servername, RPL_STATSLINKINFO, target,
				it->second.linkname + " " +
				std::to_string(it->second.wrbuf.size()) + " " +
				std::to_string(it->second.sentmsgs) + " " +
				std::to_string(it->second.sentbytes/* / 1024*/) + " " +
				std::to_string(it->second.recvmsgs) + " " +
				std::to_string(it->second.recvbytes/* / 1024*/) + " " +
				std::to_string(ft_getcurrenttime() - it->second.dtopened), "");
	}
	if (mode == "m")
	{
		for (size_t i = 0; i < 10; i++)
			reply += ft_buildmsg(serv->servername, RPL_STATSCOMMANDS, target,
				std::string("COMMANDNAME") + " " +
				"<count>" + " " +
				"<byte count>" + " " +
				"<remote count>", "");
	}
	if (mode == "o")
	{
		for (t_opit it = serv->oper.begin(); it != serv->oper.end(); it++)
			reply += ft_buildmsg(serv->servername, RPL_STATSOLINE, target,
				std::string("O") + " " + it->hostmask + " * " + it->name, "");
	}
	if (mode == "u")
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
	std::string	nick;
	t_citer		it;

	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (it != serv->clients.end())
		nick = it->getnick();
	else if (split[0][0] == ':')
		nick = split[0].substr(1);
	else
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
			ERR_NOTREGISTERED, "", "", "You have not registered");

	if (serv->fds[fd].type != FD_SERVER && (split.size() < 3 ||
		(split.size() > 2 && getservernamebymask(serv, split[2]) == serv->servername)))
	{
		if (split.size() < 2)
			serv->fds[fd].wrbuf += reply_stats(serv, nick, "*");
		else
			serv->fds[fd].wrbuf += reply_stats(serv, nick, split[1]);
	}
	else if (serv->fds[fd].type != FD_SERVER && split.size() > 2)
	{
		int	servfd = getserverfdbymask(serv, split[2]);
		if (servfd > 0)
			serv->fds[servfd].wrbuf += ":" + nick + " STATS " +
				split[1][0] + " " + split[2] + CRLF;
		else
			serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
				ERR_NOSUCHSERVER, nick, split[2], "No such server");
	}
	else if (split.size() >= 4)	// from another server: reply or forward
	{
		if (getservernamebymask(serv, split[3]) == serv->servername)
			serv->fds[fd].wrbuf += reply_stats(serv, nick, split[2]);
		else
		{
			int	servfd = getserverfdbymask(serv, split[3]);
			if (servfd > 0)
				serv->fds[servfd].wrbuf += split[0] + " STATS " +
					split[2][0] + " " + split[3] + CRLF;
			else
				serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
					ERR_NOSUCHSERVER, nick, split[3], "No such server");
		}
	}
}
