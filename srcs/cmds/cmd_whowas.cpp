/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_whowas.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/05 16:43:19 by salec             #+#    #+#             */
/*   Updated: 2021/01/15 22:34:23 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

/*
	Command: WHOWAS
	Parameters: <nickname> *( "," <nickname> ) [ <count> [ <target> ] ]

	Whowas asks for information about a nickname which no longer exists.
	This may either be due to a nickname change or the user leaving IRC.
	In response to this query, the server searches through its nickname
	history, looking for any nicks which are lexically the same (no wild
	card matching here).  The history is searched backward, returning the
	most recent entry first.  If there are multiple entries, up to
	<count> replies will be returned (or all of them if no <count>
	parameter is given).  If a non-positive number is passed as being
	<count>, then a full search is done.

	Wildcards are allowed in the <target> parameter.

	Numeric Replies:
		ERR_NONICKNAMEGIVEN
		ERR_WASNOSUCHNICK
		RPL_WHOWASUSER
		RPL_WHOISSERVER
		RPL_ENDOFWHOWAS

	Examples:
	WHOWAS Wiz					; return all information in the nick
								history about nick "WiZ";
	WHOWAS Mermaid 9			; return at most, the 9 most recent
								entries in the nick history for "Mermaid";
	WHOWAS Trillian 1 *.edu		; return the most recent history for "Trillian"
								from the first server to match "*.edu".
*/

typedef std::list<t_whowas>::iterator	t_whowasit;
typedef t_strvect::iterator				t_strvectit;

std::string	reply_whowas(IRCserv *serv, std::string const &target,
	std::string const &nicks, std::string const &entrycountstr = "")
{
	if (nicks.empty())
		return (ft_buildmsg(serv->servername,
			ERR_NONICKNAMEGIVEN, target, "", "No nickname given"));

	size_t	entrycount = -1;
	if (!entrycountstr.empty() &&
		entrycountstr.find_first_not_of("0123456789") == std::string::npos)
	{
		try { entrycount = STOUL(entrycountstr); }
		catch (std::exception const &e) { (void)e; }
	}
	if (entrycount == 0)
		entrycount = -1;

	std::string	reply = "";
	t_strvect	nvect = ft_splitstringbyany(nicks, ",");
	for (t_strvectit nvit = nvect.begin(); nvit != nvect.end(); nvit++)
	{
		size_t	count = 0;
		for (t_whowasit it = serv->nickhistory.begin();
			it != serv->nickhistory.end() && count < entrycount; it++)
		{
			if (*nvit == it->nickname)
			{
				reply += ft_buildmsg(serv->servername, RPL_WHOWASUSER, target,
					it->nickname + " " + it->username + " " + it->hostname + " *",
					it->realname);
				reply += ft_buildmsg(serv->servername,
					RPL_WHOISSERVER, target, it->nickname + " " + it->servername,
					ft_timetostring(it->dtloggedin));
				count++;
			}
		}
		if (count == 0)
			reply += ft_buildmsg(serv->servername,
				ERR_WASNOSUCHNICK, target, *nvit, "There was no such nickname");
	}
	reply += ft_buildmsg(serv->servername, RPL_ENDOFWHOWAS, target, nicks,
		"End of WHOWAS");
	return (reply);
}

void		cmd_whowas(int fd, const t_strvect &split, IRCserv *serv)
{
	t_fd		&fdref = serv->fds[fd];
	std::string	nick = getnicktoreply(fd, split, serv);
	if (nick.empty())
	{
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NOTREGISTERED, "", "", "You have not registered");
		return ;
	}

	if (fdref.type != FD_SERVER && (split.size() < 4 ||
		(split.size() > 3 && getservernamebymask(serv, split[3]) == serv->servername)))
	{
		if (split.size() < 2)
			fdref.wrbuf += reply_whowas(serv, nick, "");
		else if (split.size() > 2)
			fdref.wrbuf += reply_whowas(serv, nick, split[1], split[2]);
		else
			fdref.wrbuf += reply_whowas(serv, nick, split[1]);
	}
	else if (fdref.type != FD_SERVER && split.size() > 3)
	{
		int	servfd = getserverfdbymask(serv, split[3]);
		if (servfd > 0)
			serv->fds[servfd].wrbuf += ":" + nick + " WHOWAS " +
				split[1] + " " + split[2] + " " + split[3] + CRLF;
		else
			fdref.wrbuf += ft_buildmsg(serv->servername,
				ERR_NOSUCHSERVER, nick, split[3], "No such server");
	}
	else if (split.size() >= 5)	// from another server: reply or forward
	{
		if (getservernamebymask(serv, split[4]) == serv->servername)
			fdref.wrbuf += reply_whowas(serv, nick, split[2], split[3]);
		else
		{
			int	servfd = getserverfdbymask(serv, split[3]);
			if (servfd > 0)
				serv->fds[servfd].wrbuf += split[0] + " WHOWAS " +
					split[2] + " " + split[3] + " " + split[4] + CRLF;
			else
				fdref.wrbuf += ft_buildmsg(serv->servername,
					ERR_NOSUCHSERVER, nick, split[4], "No such server");
		}
	}
}
