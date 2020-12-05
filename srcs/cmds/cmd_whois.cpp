/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_whois.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/05 16:43:22 by salec             #+#    #+#             */
/*   Updated: 2020/12/05 17:50:39 by salec            ###   ########.fr       */
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
	specific server.  It is useful if you want to know how long the user
	in question has been idle as only local server (i.e., the server the
	user is directly connected to) knows that information, while
	everything else is globally known.
	Wildcards are allowed in the <target> parameter.

	Numeric Replies:
		ERR_NOSUCHSERVER
		ERR_NONICKNAMEGIVEN
		RPL_WHOISUSER
		RPL_WHOISCHANNELS
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

typedef std::list<Client>::iterator	t_cvit;

void	cmd_whois(int fd, const t_strvect &split, IRCserv *serv)
{
	t_citer		cit = ft_findclientfd(serv->clients.begin(),
		serv->clients.end(), fd);

	if (cit == serv->clients.end())
		return ;

	if (split.size() < 2)
	{
		// may be different
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
			ERR_NONICKNAMEGIVEN, cit->getnickname(), "", "No nickname given");
		return ;
	}
	// not done
}
