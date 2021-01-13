/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircbot.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/09 16:37:43 by salec             #+#    #+#             */
/*   Updated: 2021/01/13 19:59:52 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircbot.hpp"
#include "jsonparser.hpp"
#include <unistd.h>

int		main(int argc, char **argv)
{
	t_strvect	split;
	split.push_back(":replyto");
	split.push_back("SQUERY");
	split.push_back("botname");

	std::string	target = split[0].substr(1);
	if (argc > 1)
		split.push_back(argv[1]);
	else
		split.push_back("!weather Moscow");

	if (split.size() < 4)
		return (1);

	std::string	rplcmd = "NOTICE";
	if (split[1] == "PRIVMSG")
		rplcmd = split[1];

	split = ft_splitstringbyany(split[3], " ");
	std::string	res = cmd_weather(split);

	if (res.size() > 0)
	{
		std::string	reply;
		split = ft_splitstringbyany(res, "\n");
		for (size_t i = 0; i < split.size(); i++)
			reply += rplcmd + " " + target + " :" + split[i] + CRLF;
		write(1, reply.c_str(), reply.size());
	}

	return (0);
}
