/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_lusers.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/19 15:04:36 by salec             #+#    #+#             */
/*   Updated: 2020/12/21 15:36:25 by salec            ###   ########.fr       */
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
		RPL_LUSERUNKNOWN
		RPL_LUSERCHANNELS
		RPL_LUSERME
		ERR_NOSUCHSERVER
*/

size_t		getusercount(IRCserv *serv, std::string const &mask = "*",
	bool opersonly = false)
{
	size_t	count = 0;

	if (match(serv->servername, mask))
		for (t_citer cit = serv->clients.begin(); cit != serv->clients.end(); cit++)
			if (!opersonly || (opersonly && cit->isOperator()))
				count++;
	for (t_netit sit = serv->network.begin(); sit != serv->network.end(); sit++)
		if (match(sit->servername, mask))
			for (t_citer cit = sit->clients.begin(); cit != sit->clients.end(); cit++)
				if (!opersonly || (opersonly && cit->isOperator()))
					count++;
	return (count);
}

size_t		getservcount(IRCserv *serv, std::string const &mask = "*",
	bool connectedtomeonly = false)
{
	size_t	count = 0;

	if (!connectedtomeonly && match(serv->servername, mask))
		count++;
	for (t_netit sit = serv->network.begin(); sit != serv->network.end(); sit++)
		if ((!connectedtomeonly || sit->hopcount == 1) && match(sit->servername, mask))
			count++;
	return (count);
}

size_t		getunknownconnectscount(IRCserv *serv)
{
	size_t	unkncount = serv->fds.size() - serv->clients.size() - serv->network.size();
	for (std::unordered_map<int, t_fd>::iterator fit = serv->fds.begin(); fit != serv->fds.end(); fit++)
		if (fit->second.type == FD_ME)
			unkncount--;
	return (unkncount);
}

std::string	reply_lusers(IRCserv *serv, std::string const &target, std::string const &mask)
{
	std::string	reply = "";
	size_t	usercount = getusercount(serv, mask);
	size_t	botscount = 0;		// services
	size_t	servcount = getservcount(serv, mask);
	size_t	opercount = getusercount(serv, mask, true);
	size_t	unkncount = getunknownconnectscount(serv);
	size_t	chancount = serv->channels.size();
	size_t	svmecount = getservcount(serv, mask, true);
	size_t	clmecount = 0;
	if (match(serv->servername, mask))
		clmecount = serv->clients.size();

//  MUST send back RPL_LUSERCLIENT and RPL_LUSERME. others only when non-zero

	reply += ft_buildmsg(serv->servername, RPL_LUSERCLIENT, target, "", "There are " +
		std::to_string(usercount) + " users and " +
		std::to_string(botscount) + " services on " +
		std::to_string(servcount) + " servers");

	if (opercount != 0)
		reply += ft_buildmsg(serv->servername, RPL_LUSEROP, target,
			std::to_string(opercount), "operator(s) online");
	if (unkncount != 0)
		reply += ft_buildmsg(serv->servername, RPL_LUSERUNKNOWN, target,
			std::to_string(unkncount), "unknown connection(s)");
	if (chancount != 0)
		reply += ft_buildmsg(serv->servername, RPL_LUSERCHANNELS, target,
			std::to_string(chancount), "channels formed");

	reply += ft_buildmsg(serv->servername, RPL_LUSERME, target, "", "I have " +
		std::to_string(clmecount) + " clients and " +
		std::to_string(svmecount) + " servers");

	return (reply);
}

void		cmd_lusers(int fd, const t_strvect &split, IRCserv *serv)
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
			serv->fds[fd].wrbuf += reply_lusers(serv, nick);
		else
			serv->fds[fd].wrbuf += reply_lusers(serv, nick, split[1]);
	}
	else if (serv->fds[fd].type != FD_SERVER && split.size() > 2)
	{
		int	servfd = getserverfdbymask(serv, split[2]);
		if (servfd > 0)
			serv->fds[servfd].wrbuf += ":" + nick + " LUSERS " +
				split[1] + " " + split[2] + CRLF;
		else
			serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
				ERR_NOSUCHSERVER, nick, split[2], "No such server");
	}
	else if (split.size() >= 4)	// from another server: reply or forward
	{
		if (getservernamebymask(serv, split[3]) == serv->servername)
			serv->fds[fd].wrbuf += reply_lusers(serv, nick, split[2]);
		else
		{
			int	servfd = getserverfdbymask(serv, split[3]);
			if (servfd > 0)
				serv->fds[servfd].wrbuf += split[0] + " LUSERS " +
					split[2] + " " + split[3] + CRLF;
			else
				serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
					ERR_NOSUCHSERVER, nick, split[3], "No such server");
		}
	}
}
