/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_tools.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/11 17:08:35 by gbright           #+#    #+#             */
/*   Updated: 2020/11/21 17:39:42 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include <fstream>

std::string		reply_motd(IRCserv *serv, std::string const &nick)
{
	std::string		reply;
	std::string		motdstr;
	std::ifstream	motd(serv->motd_path);

	if (motd.is_open())
	{
		reply = ft_buildmsg(serv->servername, RPL_MOTDSTART, nick,
			"", serv->servername + " Message of the day");
		while (!motd.eof())
		{
			std::getline(motd, motdstr);
			if (motd.eof() && motdstr.length() == 0)
				break ;
			reply += ft_buildmsg(serv->servername, RPL_MOTD, nick,
				"", motdstr);
		}
		reply += ft_buildmsg(serv->servername, RPL_ENDOFMOTD, nick,
			"", "End of MOTD command");
		motd.close();
	}
	else
		reply = ft_buildmsg(serv->servername, ERR_NOMOTD, nick,
			"", "MOTD File is missing");
	return (reply);
}

std::string		reply_welcome(IRCserv *serv, std::string const &nick)
{
	std::string	reply;

	reply += ft_buildmsg(serv->servername, RPL_WELCOME,
		nick, "", "Welcome to the Internet Relay Network " + nick + "!" +
		nick + "@" + serv->servername);
	reply += ft_buildmsg(serv->servername, RPL_YOURHOST, nick, "",
		"Your host is " + serv->servername + ", running version " +
		/*serv->version*/ "0.1");
	reply += ft_buildmsg(serv->servername, RPL_CREATED, nick, "",
		"This server was created <date>");
	reply += ft_buildmsg(serv->servername, RPL_MYINFO, nick, "",
		serv->servername + " " + /*serv->version*/ "0.1" + " " +
		"<available user modes>" + " " + "<available channel modes>");
	reply += reply_motd(serv, nick);
	return (reply);
}
