/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_tools.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/11 17:08:35 by gbright           #+#    #+#             */
/*   Updated: 2020/11/26 17:04:11 by salec            ###   ########.fr       */
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
		serv->version);
	reply += ft_buildmsg(serv->servername, RPL_CREATED, nick, "",
		"This server was created <date>");
	reply += ft_buildmsg(serv->servername, RPL_MYINFO, nick, "",
		serv->servername + " " + serv->version + " " +
		"<available user modes>" + " " + "<available channel modes>");
	reply += reply_motd(serv, nick);
	return (reply);
}

bool	is_server_registred(const std::string &name, IRCserv *serv)
{
	std::vector<t_server>::iterator nearest = serv->network.begin();
	std::list<t_server_intro>::iterator server_intro;
	std::vector<t_server>::iterator end = serv->network.end();

	while (nearest != end)
	{
		if (nearest->servername == name)
			return true;
		server_intro = nearest->routing.begin();
		while (server_intro != nearest->routing.end())
		{
			if (server_intro->servername == name || server_intro->behind == name)
				return true;
			server_intro++;
		}
		nearest++;
	}
	return false;
}
