/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_tools.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/11 17:08:35 by gbright           #+#    #+#             */
/*   Updated: 2020/11/14 18:30:26 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include <fstream>

std::string		reply_motd(IRCserv *serv, t_citer const &it)
{
	std::string		reply;
	std::string		motdstr;
	std::ifstream	motd("./conf/motd.txt");
	// we can unhardcode motd file by using config but for now it's hardcoded

	if (motd.is_open())
	{
		reply = ft_buildmsg(serv->servername, RPL_MOTDSTART, it->getnickname(),
			"", serv->servername + " Message of the day");
		while (!motd.eof())
		{
			std::getline(motd, motdstr);
			if (motd.eof() && motdstr.length() == 0)
				break ;
			reply += ft_buildmsg(serv->servername, RPL_MOTD, it->getnickname(),
				"", motdstr);
		}
		reply += ft_buildmsg(serv->servername, RPL_ENDOFMOTD, it->getnickname(),
			"", "End of MOTD command");
		motd.close();
	}
	else
		reply = ft_buildmsg(serv->servername, ERR_NOMOTD, it->getnickname(),
			"", "MOTD File is missing");
	return (reply);
}

std::string		reply_welcome(IRCserv *serv, t_citer const &it)
{
	std::string	reply;

	reply += ft_buildmsg(serv->servername, RPL_WELCOME,
		it->getnickname(), "",
		"Welcome to the Internet Relay Network " +
		it->getnickname() + "!" + it->getnickname() + "@" +
		serv->servername);
	reply += ft_buildmsg(serv->servername, RPL_YOURHOST,
		it->getnickname(), "", "Your host is " + serv->servername +
		", running version " + /*serv->version*/ "0.1");
	reply += ft_buildmsg(serv->servername, RPL_CREATED,
		it->getnickname(), "", "This server was created <date>");
	reply += ft_buildmsg(serv->servername, RPL_MYINFO,
		it->getnickname(), "", serv->servername + " " +
		/*serv->version*/ "0.1" + " " +
		"<available user modes>" + " " +
		"<available channel modes>");
	reply += reply_motd(serv, it);
	return (reply);
}
