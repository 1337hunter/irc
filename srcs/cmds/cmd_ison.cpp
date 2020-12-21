/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_ison.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/17 20:31:42 by salec             #+#    #+#             */
/*   Updated: 2020/12/21 16:15:45 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

/*
	Command: ISON
	Parameters: <nickname> *( SPACE <nickname> )

	The ISON command was implemented to provide a quick and efficient
	means to get a response about whether a given nickname was currently
	on IRC. ISON only takes one (1) type of parameter: a space-separated
	list of nicks.  For each nickname in the list that is present, the
	server adds that to its reply string.  Thus the reply string may
	return empty (none of the given nicks are present), an exact copy of
	the parameter string (all of them present) or any other subset of the
	set of nicks given in the parameter.  The only limit on the number of
	nicks that may be checked is that the combined length MUST NOT be too
	large as to cause the server to chop it off so it fits in 512
	characters.

	ISON is only processed by the server local to the client sending the
	command and thus not passed onto other servers for further
	processing.

	Numeric Replies:
		RPL_ISON
		ERR_NEEDMOREPARAMS

	Example:
	ISON phone trillian WiZ jarlek		; Sample ISON request for 4 nicks.
*/

typedef t_strvect::const_iterator	t_svectit;

std::string	reply_ison(IRCserv *serv, std::string const &target,
	t_strvect const &nicks)
{
	size_t		len = 8 + serv->servername.size() + target.size();
	std::string	reply = "";
	std::string	tmp;

	for (t_svectit it = nicks.begin(); it != nicks.end(); it++)
	{
		if (ft_findnick(serv->clients.begin(), serv->clients.end(), *it) !=
			serv->clients.end())
		{
			tmp = *it;
			len += tmp.size() + 1;
			if (len <= BUF_SIZE)
				reply += tmp + " ";
			else
				break ;
		}
	}
	return (ft_buildmsg(serv->servername, RPL_ISON, target, "", reply));
}

void	cmd_ison(int fd, const t_strvect &split, IRCserv *serv)
{
	std::string	nick;
	t_citer		it;
	t_fd		&fdref = serv->fds[fd];

	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (it != serv->clients.end())
		nick = it->getnick();
	else
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NOTREGISTERED, "", "", "You have not registered");

	if (split.size() < 2)
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NEEDMOREPARAMS, nick, "ISON", "Not enough parameters");
	else
		fdref.wrbuf += reply_ison(serv, nick,
			t_strvect(split.begin() + 1, split.end()));
}
