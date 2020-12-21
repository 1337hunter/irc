/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_whois.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/05 16:43:22 by salec             #+#    #+#             */
/*   Updated: 2020/12/21 18:47:28 by salec            ###   ########.fr       */
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

void	cmd_whois(int fd, const t_strvect &split, IRCserv *serv)
{
	t_fd		&fdref = serv->fds[fd];
	std::string	nick = getnicktoreply(fd, split, serv);
	if (nick.empty())
	{
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NOTREGISTERED, "", "", "You have not registered");
		return ;
	}

	std::string	servername = "";
	std::string	nickname = "";

	if (split.size() < 2)
	{
		// may be different
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NONICKNAMEGIVEN, nick, "", "No nickname given");
		return ;
	}

	if (split.size() > 2)
	{
		nickname = split[2];

		// for some stupid reason Textual sends WHOIS nick nick
		// so i implement stupid solution to this
		if (nickname != split[1])
		{
			servername = getservernamebymask(serv, split[1]);
			if (servername.empty())
			{
				fdref.wrbuf += ft_buildmsg(serv->servername,
					ERR_NOSUCHSERVER, nick, split[1], "No such server");
				return ;
			}
		}
	}
	else
		nickname = split[1];

	t_cvit it = serv->clients.begin();
	for (; it != serv->clients.end(); it++)
	{
		if (match(it->getnick(), nickname) &&
			(servername.empty() || (servername == serv->servername)))
		{
			fdref.wrbuf += ft_buildmsg(serv->servername,
				RPL_WHOISUSER, nick,
				it->getnick() + " " + it->getusername() + " " +
				it->gethostname() + " *", it->getrealname());

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
				fdref.wrbuf += ft_buildmsg(serv->servername,
					RPL_WHOISCHANNELS, nick,
					it->getnick() + " ", channelstr);

			/* unreal also sends
				379 nickname nickname is using modes +iwx
				378 nickname nickname is connecting from *@localhost 127.0.0.1
			*/

			fdref.wrbuf += ft_buildmsg(serv->servername,
				RPL_WHOISSERVER, nick,
				it->getnick() + " " + serv->servername, serv->info);
			if (it->isOperator())
				fdref.wrbuf += ft_buildmsg(serv->servername,
					RPL_WHOISOPERATOR, nick,
					it->getnick(), "is an IRC operator");
			/*
			if (it->isAway())
				fdref.wrbuf += ft_buildmsg(serv->servername,
					RPL_AWAY, nick, it->getnick(), "<away message>");
			*/

			fdref.wrbuf += ft_buildmsg(serv->servername,
				RPL_WHOISIDLE, nick, it->getnick() + " " +
				std::to_string(ft_getcurrenttime() - it->gettimeloggedin()),
				"seconds idle");
			/* unreal also sends timestamp of login in 317 */

			break ;
		}
	}
	if (it == serv->clients.end())
	{
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NOSUCHNICK, nick, nickname, "No such nick/channel");
	}
	fdref.wrbuf += ft_buildmsg(serv->servername,
		RPL_ENDOFWHOIS, nick, nickname, "End of WHOIS list");
}
