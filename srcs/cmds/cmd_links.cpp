#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

/*
	Command: LINKS
	Parameters: [ [ <remote server> ] <server mask> ]

	With LINKS, a user can list all servernames, which are known by the
	server answering the query.  The returned list of servers MUST match
	the mask, or if no mask is given, the full list is returned.
	If <remote server> is given in addition to <server mask>, the LINKS
	command is forwarded to the first server found that matches that name
	(if any), and that server is then required to answer the query.

	Numeric Replies:
		ERR_NOSUCHSERVER
		RPL_LINKS
		RPL_ENDOFLINKS

	Examples:
	LINKS *.au				; Command to list all servers which
							have a name that matches *.au;
	LINKS *.edu *.bu.edu	; Command to list servers matching *.bu.edu
							as seen by the first server matching *.edu.
*/

typedef std::list<t_server_intro>::iterator	t_netit2;


std::string	reply_links(IRCserv *serv, std::string const &target,
	std::string const &mask)
{
	std::string	reply = "";

	if (match(serv->servername, mask))
		reply += ft_buildmsg(serv->servername, RPL_LINKS, target,
			mask + " " + serv->servername, TOSTRING(0) + " " + serv->info);
	for (t_netit sit = serv->network.begin(); sit != serv->network.end(); sit++)
	{
		if (match(sit->servername, mask))
			reply += ft_buildmsg(serv->servername, RPL_LINKS, target,
				mask + " " + sit->servername,
				TOSTRING(sit->hopcount) + " " + sit->info);
		for (t_netit2 ssit = sit->routing.begin(); ssit != sit->routing.end(); ssit++)
			if (match(ssit->servername, mask))
				reply += ft_buildmsg(serv->servername, RPL_LINKS, target,
					mask + " " + ssit->servername,
					TOSTRING(ssit->hopcount) + " " + ssit->info);
	}
	reply += ft_buildmsg(serv->servername, RPL_ENDOFLINKS, target, mask,
		"End of LINKS list");
	return (reply);
}

void	cmd_links(int fd, const t_strvect &split, IRCserv *serv)
{
	t_fd		&fdref = serv->fds[fd];
	std::string	nick = getnicktoreply(fd, split, serv);
	if (nick.empty())
	{
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NOTREGISTERED, "", "", "You have not registered");
		return ;
	}

	if (fdref.type != FD_SERVER && (split.size() < 3 ||
		(split.size() > 2 && getservernamebymask(serv, split[1]) == serv->servername)))
	{
		if (split.size() < 2)
			fdref.wrbuf += reply_links(serv, nick, "*");
		else if (split.size() > 2)
			fdref.wrbuf += reply_links(serv, nick, split[2]);
		else
			fdref.wrbuf += reply_links(serv, nick, split[1]);
	}
	else if (fdref.type != FD_SERVER && split.size() > 2)
	{
		int	servfd = getserverfdbymask(serv, split[1]);
		if (servfd > 0)
			serv->fds[servfd].wrbuf += ":" + nick + " LINKS " +
				split[1] + " " + split[2] + CRLF;
		else
			fdref.wrbuf += ft_buildmsg(serv->servername,
				ERR_NOSUCHSERVER, nick, split[1], "No such server");
	}
	else if (split.size() >= 4)	// from another server: reply or forward
	{
		if (getservernamebymask(serv, split[2]) == serv->servername)
			fdref.wrbuf += reply_links(serv, nick, split[3]);
		else
		{
			int	servfd = getserverfdbymask(serv, split[2]);
			if (servfd > 0)
				serv->fds[servfd].wrbuf += split[0] + " LINKS " +
					split[2] + " " + split[3] + CRLF;
			else
				fdref.wrbuf += ft_buildmsg(serv->servername,
					ERR_NOSUCHSERVER, nick, split[2], "No such server");
		}
	}
}
