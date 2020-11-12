/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_tools.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/11 17:08:35 by gbright           #+#    #+#             */
/*   Updated: 2020/11/12 21:54:06 by salec            ###   ########.fr       */
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
		reply = ft_buildmsg(serv->hostname, RPL_MOTDSTART, it->getnickname(),
			"", serv->hostname + " Message of the day");
		while (!motd.eof())
		{
			std::getline(motd, motdstr);
			if (motd.eof() && motdstr.length() == 0)
				break ;
			reply += ft_buildmsg(serv->hostname, RPL_MOTDSTART, it->getnickname(),
				"", motdstr);
		}
		reply += ft_buildmsg(serv->hostname, RPL_ENDOFMOTD, it->getnickname(),
			"", "End of MOTD command");
		motd.close();
	}
	else
		reply = ft_buildmsg(serv->hostname, ERR_NOMOTD, it->getnickname(),
			"", "MOTD File is missing");
	return (reply);
}

std::string		reply_welcome(IRCserv *serv, t_citer const &it)
{
	std::string	reply;

	reply += ft_buildmsg(serv->hostname, RPL_WELCOME,
		it->getnickname(), "",
		"Welcome to the Internet Relay Network " +
		it->getnickname() + "!" + it->getnickname() + "@" +
		serv->hostname);
	reply += ft_buildmsg(serv->hostname, RPL_YOURHOST,
		it->getnickname(), "", "Your host is " + serv->hostname +
		", running version " + /*serv->version*/ "0.1");
	reply += ft_buildmsg(serv->hostname, RPL_CREATED,
		it->getnickname(), "", "This server was created <date>");
	reply += ft_buildmsg(serv->hostname, RPL_MYINFO,
		it->getnickname(), "", serv->hostname + " " +
		/*serv->version*/ "0.1" + " " +
		"<available user modes>" + " " +
		"<available channel modes>");
	reply += reply_motd(serv, it);
	return (reply);
}
