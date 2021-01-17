/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msgtools.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/14 18:56:51 by salec             #+#    #+#             */
/*   Updated: 2021/01/17 16:50:03 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircbot.hpp"
#include <sys/socket.h>

bool	checkreplycode(t_strvect const &split, ircbot const &bot)
{
	if (split[1] ==  "431" || split[1] == "432" || split[1] == "433" ||
		split[1] == "436" || split[1] == "462")
	{
		std::cout << bot.botname << " got reply code " << split[1] <<
			": " << split[2];
		if (split.size() > 3)
			std::cout << " - " << split[3];
		std::cout << std::endl;

		if (split[1] ==  "431" || split[1] == "432" || split[1] == "433")
		{
			std::cout << "Please enter new bot nickname (up to 9 symbols): ";
			std::string	reply = "";
			std::getline(std::cin, reply);
			if (bot.asService)
				reply = "SERVICE " + reply + " * * 0 0 :ircbot v" + VERSION + CRLF;
			else
				reply = "NICK " + reply + CRLF;
			if (send(bot.sock, reply.c_str(), reply.size(), 0) < 0)
				ioerror(bot.sock, "send error");
		}
		else
			ioerror(bot.sock, "got error code from server " + split[1]);
	}
	if (split[1] == "002")
	{
		std::cout << "IRC server registration success." << std::endl;
		if (split.size() > 2)
			std::cout << split[3] << std::endl;
	}
	return (false);
}

bool	checkmsg(t_strvect const &split, ircbot const &bot)
{
	if (split.size() < 2)
		return (false);

	size_t	i = 0;
	if (split[0][0] == ':')
		i = 1;
	std::string	tmp = ft_strtoupper(split[i]);
	if (split.size() > i + 1 && tmp == "PING")
	{
		std::string	reply = "PONG :" + split[i + 1] + CRLF;
		if (send(bot.sock, reply.c_str(), reply.size(), 0) < 0)
			ioerror(bot.sock, "send error");
		return (false);
	}

	// got an error message
	if (split.size() > i + 1 && tmp == "ERROR")
	{
		std::cout << "ERROR message: " << split[i + 1] << std::endl;
		return (false);
	}

	// got a reply code
	if (split.size() >= 3 && split[1].size() == 3 &&
		split[1].find_first_not_of("0123456789") == std::string::npos)
		return (checkreplycode(split, bot));

	if (split.size() < 4 ||
		(tmp != "SQUERY" && tmp != "PRIVMSG" && tmp != "NOTICE"))
		return (false);

	return (true);
}
