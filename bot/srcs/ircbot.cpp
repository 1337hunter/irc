#include "ircbot.hpp"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>

#define BUF_SIZE	512

ircbot::ircbot(int sock, bool asService) : sock(sock), asService(asService)
{
	botname = BOTNAME;
	version = VERSION;
	prefix = PREFIX;
	cmds["help"]	= Command(cmd_help, "get commands help");
	cmds["info"]	= Command(cmd_info, "get bot info");
	cmds["version"]	= Command(cmd_version, "get bot version");
	cmds["weather"]	= Command(cmd_weather,
		std::string("get current weather.\nDefault location: Moscow,RU. ") +
		"Example: " + prefix + "weather London");
}

ircbot::~ircbot() {}

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
