/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_whowas.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/05 16:43:19 by salec             #+#    #+#             */
/*   Updated: 2020/12/05 17:59:34 by salec            ###   ########.fr       */
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

typedef std::vector<t_whowas>::iterator	t_whowasit;
typedef std::vector<t_link>::iterator	t_linkit;

void	cmd_whowas(int fd, const t_strvect &split, IRCserv *serv)
{
	t_citer		cit = ft_findclientfd(serv->clients.begin(),
		serv->clients.end(), fd);
	size_t		entrycount = -1;
	std::string	servername = "";
	std::string	nickname = "";

	if (cit == serv->clients.end())
		return ;
	if (split.size() < 2)
	{
		// may be different
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
			ERR_NONICKNAMEGIVEN, cit->getnickname(), "", "No nickname given");
		return ;
	}
	nickname = split[1];
	if (split.size() > 2)
	{
		if (split[2].find_first_not_of("0123456789") == std::string::npos)
		{
			try
			{
				size_t tmp = std::stoul(split[2]);
				if (tmp != 0)
					entrycount = tmp;
			}
			catch (std::exception const &e) { (void)e; }
		}
	}

	if (split.size() > 3)
	{
		t_linkit servit = serv->link.begin();
		if (match(serv->servername, split[3]))
		{
			servername = serv->servername;
			servit = serv->link.end();
		}
		for (; servit != serv->link.end(); servit++)
		{
			if (match(servit->servername, split[3]))
			{
				servername = servit->servername;
				break ;
			}
		}
		if (servit == serv->link.end() && servername.empty())
		{
			// this is what insp sends.
			// RFC does not specify the ERR_NOSUCHSERVER for this query
			// but it <<<SEEMS>>> pretty logical
			serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
				ERR_NOSUCHSERVER, cit->getnickname(),
				split[3], "No such server");
			return ;
		}
	}

	size_t	count = 0;
	for (t_whowasit it = serv->nickhistory.begin();
		it != serv->nickhistory.end() && count < entrycount; it++)
	{
		if (nickname == it->nickname &&
			(servername.empty() || (servername == it->servername)))
		{
			serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
				RPL_WHOWASUSER, cit->getnickname(),
				nickname + " " + it->username + " " + it->hostname + " *",
				it->realname);
			serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
				RPL_WHOISSERVER, cit->getnickname(),
				nickname + " " + it->servername,
				ft_timetostring(it->dtloggedin));
			count++;
		}
	}
	if (count == 0)
	{
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
			ERR_WASNOSUCHNICK, cit->getnickname(),
			nickname, "There was no such nickname");
	}
	serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
		RPL_ENDOFWHOWAS, cit->getnickname(), nickname, "End of WHOWAS");
}
