/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_who.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/26 15:53:32 by salec             #+#    #+#             */
/*   Updated: 2020/12/25 20:07:17 by salec            ###   ########.fr       */
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
	the client.
	In the absence of the <mask> parameter, all visible users are listed.
	The same result can be achieved by using a <mask> of "0" or
	any wildcard which will end up matching every visible user.

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

typedef std::list<Client>::iterator		t_cit;
typedef std::list<Channel>::iterator	t_chit;
typedef std::vector<t_server>::iterator	t_netit;
typedef std::MAP<Client*, client_flags>::iterator	t_cpit;

std::string	getservernamebytoken(IRCserv *serv, std::string const &token)
{
	if (serv->token == token)
		return (serv->servername);
	else
		for (t_netit nit = serv->network.begin(); nit != serv->network.end(); nit++)
			if (nit->token == token)
				return (nit->servername);
	return ("*");
}

std::string	reply_who(IRCserv *serv, std::string const &target,
	std::string const &mask = "*", bool oponly = false)
{
	std::string	reply = "";

	t_chit chit;
	if (mask[0] == '#' || mask[0] == '!' || mask[0] == '&' || mask[0] == '+')
	{
		for (chit = serv->channels.begin(); chit != serv->channels.end(); chit++)
		{
			if (match(chit->getname(), mask) && !chit->isSecret() && !chit->isPrivate())
			{
				for (t_cpit it = chit->getclients().begin(); it != chit->getclients().end(); it++)
				{
					if (!it->first->isInvisible() &&
						(!oponly || (oponly && (it->first->isOperator() ||
						it->second._Operator || it->second._operator))))
					{
						std::string	userinfo = chit->getname() + " " +
							it->first->getusername() + " " +
							it->first->gethostname() + " " +
							getservernamebytoken(serv, it->first->gettoken()) + " " +
							it->first->getnick() + " ";
						if (it->first->isAway())
							userinfo += "G";	// gone
						else
							userinfo += "H";	// here
						if (it->first->isOperator())
							userinfo += " *";
						if (it->second._Operator || it->second._operator)
							userinfo += " @";
						else if (it->second._voice)
							userinfo += " +";
						reply += ft_buildmsg(serv->servername,
							RPL_WHOREPLY, target, userinfo,
							it->first->gethopcount() + " " +
							it->first->getrealname());
					}
				}
			}
		}
	}
	else
	{
		for (t_cit it = serv->clients.begin(); it != serv->clients.end(); it++)
		{
			//	host, server, real name and nickname matches
			if (!it->isInvisible() && (!oponly || (oponly && it->isOperator())) &&
				(match(it->getnick(), mask) || match(it->getrealname(), mask) ||
				match(it->gethostname(), mask) || match(serv->servername, mask)))
			{
				std::string	userinfo = "* " +
					it->getusername() + " " + it->gethostname() + " " +
					serv->servername + " " + it->getnick() + " ";
				if (it->isAway())
					userinfo += "G";	// gone
				else
					userinfo += "H";	// here
				if (it->isOperator())
					userinfo += " *";
				reply += ft_buildmsg(serv->servername,
					RPL_WHOREPLY, target, userinfo,
					it->gethopcount() + " " + it->getrealname());
			}
		}
		for (t_netit nit = serv->network.begin(); nit != serv->network.end(); nit++)
		{
			for (t_cit it = nit->clients.begin(); it != nit->clients.end(); it++)
			{
				if (!it->isInvisible() && (!oponly || (oponly && it->isOperator())) &&
					(match(it->getnick(), mask) || match(it->getrealname(), mask) ||
					match(it->gethostname(), mask) || match(nit->servername, mask)))
				{
					std::string	userinfo = "* " +
						it->getusername() + " " + it->gethostname() + " " +
						nit->servername + " " + it->getnick() + " ";
					if (it->isAway())
						userinfo += "G";	// gone
					else
						userinfo += "H";	// here
					if (it->isOperator())
						userinfo += " *";
					reply += ft_buildmsg(serv->servername,
						RPL_WHOREPLY, target, userinfo,
						it->gethopcount() + " " + it->getrealname());
				}
			}
		}
	}

	reply += ft_buildmsg(serv->servername,
		RPL_ENDOFWHO, target, mask, "End of WHO list");
	return (reply);
}

void		cmd_who(int fd, const t_strvect &split, IRCserv *serv)
{
	t_fd		&fdref = serv->fds[fd];
	std::string	nick = getnicktoreply(fd, split, serv);
	if (nick.empty())
	{
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NOTREGISTERED, "", "", "You have not registered");
		return ;
	}

	std::string	mask = "*";
	bool		oponly = false;
	if (split.size() > 1 && split[1] != "0" && split[1] != "*")
		mask = split[1];
	if (split.size() > 2 && split[2] == "o")
		oponly = true;
	fdref.wrbuf += reply_who(serv, nick, mask, oponly);
}
