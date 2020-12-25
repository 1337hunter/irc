/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_whois.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/05 16:43:22 by salec             #+#    #+#             */
/*   Updated: 2020/12/25 20:26:09 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

/*
	Command: WHOIS
	Parameters: [ <target> ] <mask> *( "," <mask> )

	This command is used to query information about particular user.
	The server will answer this command with several numeric messages
	indicating different statuses of each user which matches the mask (if
	you are entitled to see them).  If no wildcard is present in the
	<mask>, any information about that nick which you are allowed to see
	is presented.

	If the <target> parameter is specified, it sends the query to a
	specific server. It is useful if you want to know how long the user
	in question has been idle as only local server (i.e., the server the
	user is directly connected to) knows that information, while
	everything else is globally known.
	Wildcards are allowed in the <target> parameter.

	Numeric Replies:
		ERR_NOSUCHSERVER
		ERR_NONICKNAMEGIVEN
		RPL_WHOISUSER
		RPL_WHOISCHANNELS
		RPL_WHOISSERVER
		RPL_AWAY
		RPL_WHOISOPERATOR
		RPL_WHOISIDLE
		ERR_NOSUCHNICK
		RPL_ENDOFWHOIS

	Examples:
	WHOIS wiz					; return available user information
								about nick WiZ
	WHOIS eff.org trillian		; ask server eff.org for user
								information  about trillian
*/

typedef std::list<Client>::iterator		t_cvit;
typedef std::list<Channel*>::iterator	t_chanit;
typedef t_strvect::iterator				t_strvectit;

std::string	getwhoisinfo(IRCserv *serv, std::string const &target, t_cvit it)
{
	std::string	reply = ft_buildmsg(serv->servername,
		RPL_WHOISUSER, target, it->getnick() + " " + it->getusername() +
		" " + it->gethostname() + " *", it->getrealname());

	std::string	channelstr = "";
	for (t_chanit chit = it->getchannels().begin();
		chit != it->getchannels().end(); chit++)
	{
		if (((*chit)->isSecret()) || ((*chit)->isPrivate()))
			continue ;
		if (!channelstr.empty())
			channelstr += " ";
		if ((*chit)->getclients()[it->getptr()]._Operator ||
			(*chit)->getclients()[it->getptr()]._operator)
			channelstr += "@";
		else if ((*chit)->getclients()[it->getptr()]._voice)
			channelstr += "+";
		channelstr += (*chit)->getname();
	}
	if (!channelstr.empty())
		reply += ft_buildmsg(serv->servername, RPL_WHOISCHANNELS, target,
			it->getnick() + " ", channelstr);

	reply += ft_buildmsg(serv->servername, RPL_WHOISSERVER, target,
		it->getnick() + " " + serv->servername,serv->info);
	if (it->isOperator())
		reply += ft_buildmsg(serv->servername, RPL_WHOISOPERATOR, target,
			it->getnick(), "is an IRC operator");
	if (it->isAway())
		reply += ft_buildmsg(serv->servername, RPL_AWAY, target,
			it->getnick(), it->getAwayMsg());

	reply += ft_buildmsg(serv->servername,
		RPL_WHOISIDLE, target, it->getnick() + " " +
		TOSTRING(ft_getcurrenttime() - it->gettimelastactive()),
		"seconds idle");
	/* ircd also sends timestamp of login in 317 */
	return (reply);
}

std::string	reply_whois(IRCserv *serv, std::string const &target,
	std::string const &nicks)
{
	if (nicks.empty())
		return (ft_buildmsg(serv->servername,
			ERR_NONICKNAMEGIVEN, target, "", "No nickname given"));

	std::string	reply = "";
	t_strvect	nvect = ft_splitstringbyany(nicks, ",");
	for (t_strvectit nvit = nvect.begin(); nvit != nvect.end(); nvit++)
	{
		t_citer it;
		for (it = serv->clients.begin(); it != serv->clients.end(); it++)
		{
			if (match(it->getnick(), *nvit) && !it->isInvisible())
			{
				reply += getwhoisinfo(serv, target, it);
				break ;
			}
		}
		if (it == serv->clients.end())
			reply +=  ft_buildmsg(serv->servername,
				ERR_NOSUCHNICK, target, *nvit, "No such nick/channel");
	}
	reply += ft_buildmsg(serv->servername, RPL_ENDOFWHOIS, target,
		nicks, "End of WHOIS list");
	return (reply);
}

void		cmd_whois(int fd, const t_strvect &split, IRCserv *serv)
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
		(split.size() > 2 && getservernamebymask(serv, split[1]) == serv->servername)))
	{
		if (split.size() < 2)
			fdref.wrbuf += reply_whois(serv, nick, "");
		else if (split.size() > 2)
			fdref.wrbuf += reply_whois(serv, nick, split[2]);
		else
			fdref.wrbuf += reply_whois(serv, nick, split[1]);
	}
	else if (fdref.type != FD_SERVER && split.size() > 2)
	{
		int	servfd = getserverfdbymask(serv, split[1]);
		if (servfd > 0)
			serv->fds[servfd].wrbuf += ":" + nick + " WHOIS " +
				split[1] + " " + split[2] + CRLF;
		else
			fdref.wrbuf += ft_buildmsg(serv->servername,
				ERR_NOSUCHSERVER, nick, split[1], "No such server");
	}
	else if (split.size() >= 4)	// from another server: reply or forward
	{
		if (getservernamebymask(serv, split[2]) == serv->servername)
			fdref.wrbuf += reply_whois(serv, nick, split[3]);
		else
		{
			int	servfd = getserverfdbymask(serv, split[2]);
			if (servfd > 0)
				serv->fds[servfd].wrbuf += split[0] + " WHOIS " +
					split[2] + " " + split[3] + CRLF;
			else
				fdref.wrbuf += ft_buildmsg(serv->servername,
					ERR_NOSUCHSERVER, nick, split[2], "No such server");
		}
	}
}
