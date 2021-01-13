/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircbot.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/09 16:37:43 by salec             #+#    #+#             */
/*   Updated: 2021/01/13 21:55:23 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircbot.hpp"
#include "jsonparser.hpp"
#include <unistd.h>

ircbot::ircbot()
{
	botname = BOTNAME;
	version = VERSION;
	prefix = PREFIX;
	cmds["help"]	= Command(cmd_help, "get commands help");
	cmds["info"]	= Command(cmd_info, "get bot info");
	cmds["version"]	= Command(cmd_version, "get bot version");
	cmds["weather"]	= Command(cmd_weather,
		std::string("get current weather. Default location: Moscow,RU. ") +
		"Example: " + prefix + "weather London");
}

ircbot::~ircbot() {}

void	ProcessMessage(t_strvect const &split, ircbot const &bot)
{
	if (split.size() < 4)
		return ;
	std::string	target = split[0].substr(1);
	std::string	rplcmd = "NOTICE";
	if (split[1] == "PRIVMSG")
		rplcmd = split[1];

	t_strvect	tmp = ft_splitstringbyany(split[3], " ");

	if (tmp.size() < 1 || tmp[0].size() < 2 || tmp[0][0] != bot.prefix)
		return ;
	tmp[0] = tmp[0].substr(1);
	std::string	res = "";
	try
	{
		res = bot.cmds.at(tmp[0]).Execute(tmp, bot);
	}
	catch (std::exception const &e) { (void)e; }

	if (res.size() > 0)
	{
		std::string	reply;
		tmp = ft_splitstringbyany(res, "\n");
		for (size_t i = 0; i < tmp.size(); i++)
			reply += rplcmd + " " + target + " :" + tmp[i] + CRLF;
		write(1, reply.c_str(), reply.size());
	}

}
