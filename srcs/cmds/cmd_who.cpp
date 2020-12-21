/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_who.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/26 15:53:32 by salec             #+#    #+#             */
/*   Updated: 2020/12/21 18:46:59 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

/*	Command: WHO
	Parameters: [ <mask> [ "o" ] ]

	The WHO command is used by a client to generate a query which returns
	a list of information which 'matches' the <mask> parameter given by
	the client.  In the absence of the <mask> parameter, all visible
	(users who aren't invisible (user mode +i) and who don't have a
	common channel with the requesting client) are listed.  The same
	result can be achieved by using a <mask> of "0" or any wildcard which
	will end up matching every visible user.

	The <mask> passed to WHO is matched against users' host, server, real
	name and nickname if the channel <mask> cannot be found.

	If the "o" parameter is passed only operators are returned according
	to the <mask> supplied.

	Numeric Replies:
		ERR_NOSUCHSERVER
		RPL_WHOREPLY
		RPL_ENDOFWHO

	Examples:
	WHO *.fi		; Command to list all users who match against "*.fi".
	WHO jto* o		; Command to list all users with a match against "jto*"
					if they are an operator.
*/

typedef std::list<Client>::iterator	t_cvit;

void	cmd_who(int fd, const t_strvect &split, IRCserv *serv)
{
	std::string	mask;
	bool		opers = false;
	t_fd		&fdref = serv->fds[fd];
	std::string	nick = getnicktoreply(fd, split, serv);
	if (nick.empty())
	{
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NOTREGISTERED, "", "", "You have not registered");
		return ;
	}

	if (split.size() < 2)
	{
		mask = "*";
	}
	else
	{
		if (split[1] == "o")
		{
			mask = "*";
			opers = true;
		}
		else
		{
			mask = split[1];
			if (split.size() > 2 && split[2] == "o")
				opers = true;
		}
	}

	for (t_cvit it = serv->clients.begin(); it != serv->clients.end(); it++)
	{
		if (match(it->getnick(), mask))
		{
			if (!opers || (opers && it->isOperator()))
				fdref.wrbuf += ft_buildmsg(serv->servername,
					RPL_WHOREPLY, nick,
					"* " + it->getusername() + " " + it->gethostname() + " " +
					serv->servername + " " + it->getnick() + " H",
					"0 " + it->getrealname());
		}
	}
	fdref.wrbuf += ft_buildmsg(serv->servername,
		RPL_ENDOFWHO, nick, mask, "End of WHO list");
}
