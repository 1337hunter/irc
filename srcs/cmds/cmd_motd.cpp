#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"
#include <fstream>

/*
	Command: MOTD
	Parameters: [ <target> ]

	The MOTD command is used to get the "Message Of The Day" of the given
	server, or current server if <target> is omitted.
	Wildcards are allowed in the <target> parameter.

	Numeric Replies:
		RPL_MOTDSTART
		RPL_MOTD
		RPL_ENDOFMOTD
		ERR_NOMOTD

	When responding to the MOTD message and the MOTD file is found,
	the file is displayed line by line, with each line no longer
	than 80 characters, using RPL_MOTD format replies.
	These MUST be surrounded by a RPL_MOTDSTART (before the RPL_MOTDs)
	and an RPL_ENDOFMOTD (after).
*/

std::string		reply_motd(IRCserv *serv, std::string const &nick)
{
	std::string		reply;
	std::string		motdstr;
	std::ifstream	motd(serv->motd_path.c_str());

	if (motd.is_open())
	{
		reply = ft_buildmsg(serv->servername, RPL_MOTDSTART, nick,
			"", "- " + serv->servername + " Message of the day - ");
		while (!motd.eof())
		{
			std::getline(motd, motdstr);
			if (motd.eof() && motdstr.length() == 0)
				break ;
			if (motdstr.length() > 80)
				motdstr = motdstr.substr(0, 80);
			reply += ft_buildmsg(serv->servername, RPL_MOTD, nick,
				"", "- " + motdstr);
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

void	cmd_motd(int fd, const t_strvect &split, IRCserv *serv)
{
	t_fd		&fdref = serv->fds[fd];
	std::string	nick = getnicktoreply(fd, split, serv);
	if (nick.empty())
	{
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NOTREGISTERED, "", "", "You have not registered");
		return ;
	}

	if (fdref.type != FD_SERVER && (split.size() < 2 ||
		(split.size() > 1 && getservernamebymask(serv, split[1]) == serv->servername)))
	{
		fdref.wrbuf += reply_motd(serv, nick);
	}
	else if (fdref.type != FD_SERVER && split.size() > 1)
	{
		int	servfd = getserverfdbymask(serv, split[1]);
		if (servfd > 0)
			serv->fds[servfd].wrbuf += ":" + nick + " MOTD " + split[1] + CRLF;
	}
	else if (split.size() >= 3)	// from another server: reply or forward
	{
		nick = split[0].substr(1);
		if (getservernamebymask(serv, split[2]) == serv->servername)
			fdref.wrbuf += reply_motd(serv, nick);
		else
		{
			int	servfd = getserverfdbymask(serv, split[2]);
			if (servfd > 0)
				serv->fds[servfd].wrbuf += split[0] + " MOTD " + split[2] + CRLF;
		}
	}
}
