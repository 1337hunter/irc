/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_userhost.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/26 15:47:09 by salec             #+#    #+#             */
/*   Updated: 2020/12/21 18:31:43 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

/*	Command: USERHOST
	Parameters: <nickname> *( SPACE <nickname> )

	The USERHOST command takes a list of up to 5 nicknames, each
	separated by a space character and returns a list of information
	about each nickname that it found.  The returned list has each reply
	separated by a space.

	Numeric Replies:
		RPL_USERHOST
		ERR_NEEDMOREPARAMS

	Example:
	USERHOST Wiz Michael syrk
	; USERHOST request for information on nicks "Wiz", "Michael", and "syrk"
	:ircd.stealth.net 302 yournick :syrk=+syrk@millennium.stealth.net
	; Reply for user syrk
*/

typedef t_strvect::const_iterator	t_svectit;

std::string	reply_userhost(IRCserv *serv, std::string const &target,
	t_strvect const &nicks)
{
	std::string	reply = "";
	int			count = 0;

	for (t_svectit it = nicks.begin(); it != nicks.end() && count < 5; it++)
	{
		Client	*client = find_client_by_nick(*it, serv);
		if (client != NULL)
		{
			if (!reply.empty())
				reply += " ";
			/*
				reply = nickname [ "*" ] "=" ( "+" / "-" ) hostname
				The '*' indicates whether the client has registered
				as an Operator.  The '-' or '+' characters represent
				whether the client has set an AWAY message or not
				respectively.
			*/
			reply += client->getnick();
			if (client->isOperator())
				reply += "*";
			reply += "=";
			if (client->isAway())
				reply += "-";
			else
				reply += "+";
			reply += client->getusername() + "@" + client->gethostname();
		}
		count++;
	}
	return (ft_buildmsg(serv->servername, RPL_USERHOST, target, "", reply));
}

void		cmd_userhost(int fd, const t_strvect &split, IRCserv *serv)
{
	t_fd		&fdref = serv->fds[fd];
	std::string	nick = getnicktoreply(fd, split, serv);
	if (nick.empty())
	{
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NOTREGISTERED, "", "", "You have not registered");
		return ;
	}

	if (split.size() < 2)
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NEEDMOREPARAMS, nick, "USERHOST", "Not enough parameters");
	else
		fdref.wrbuf += reply_userhost(serv, nick,
			t_strvect(split.begin() + 1, split.end()));
}
