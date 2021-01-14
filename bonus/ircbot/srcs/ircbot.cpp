/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircbot.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/09 16:37:43 by salec             #+#    #+#             */
/*   Updated: 2021/01/14 18:35:59 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircbot.hpp"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>

#define BUF_SIZE	512

ircbot::ircbot(int sock) : sock(sock)
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

void	ioerror(int sock, std::string const &msg)
{
	close(sock);
	error_exit(msg);
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

	if (split.size() < 4 ||
		(tmp != "SQUERY" && tmp != "PRIVMSG" && tmp != "NOTICE"))
		return (false);

	return (true);
}

void	ProcessMessage(t_strvect const &split, ircbot const &bot)
{
	if (!checkmsg(split, bot))
		return ;

	std::string	target = split[0].substr(1);
	std::string	rplcmd = "NOTICE";
	if (ft_strtoupper(split[1]) == "PRIVMSG")
		rplcmd = split[1];

	t_strvect	tmp = ft_splitstringbyany(split[3], " ");

	if (tmp.size() < 1 || tmp[0].size() < 2 || tmp[0][0] != bot.prefix)
		return ;
	tmp[0] = tmp[0].substr(1);
	tmp[0] = ft_strtolower(tmp[0]);
	std::string	res = "";
	try { res = bot.cmds.at(tmp[0]).Execute(tmp, bot); }
	catch (std::exception const &e) { (void)e; }

	if (res.size() > 0)
	{
		std::string	reply;
		tmp = ft_splitstringbyany(res, "\n");
		for (size_t i = 0; i < tmp.size(); i++)
			reply += rplcmd + " " + target + " :" + tmp[i] + CRLF;
		if (send(bot.sock, reply.c_str(), reply.size(), 0) < 0)
			ioerror(bot.sock, "send error");
	}
}

void	ReceiveMessages(ircbot const &bot)
{
	ssize_t			r = 0;
	std::string		rdbuf;
	unsigned char	buf[BUF_SIZE + 1];

	while ((r = recv(bot.sock, buf, BUF_SIZE, 0)) > 0)
	{
		if (r >= 0)
			buf[r] = 0;
		rdbuf += reinterpret_cast<char*>(buf);
		if (rdbuf.find_last_of(CRLF) != std::string::npos &&
			rdbuf.find_last_of(CRLF) + 1 == rdbuf.length())
		{
			t_strvect	split = ft_splitstringbyany(rdbuf, CRLF);
			for (size_t i = 0; i < split.size(); i++)
				ProcessMessage(ft_splitcmdbyspace(split[i]), bot);
			rdbuf.erase();
		}
	}
	if (r < 0)
		ioerror(bot.sock, "recv error");
}
