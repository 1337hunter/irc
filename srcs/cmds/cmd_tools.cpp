#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"
#include <fstream>

/*
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
	std::ifstream	motd(serv->motd_path);

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

/*
	In particular the server SHALL send the current user/service/server
	count (as per the LUSER reply) and finally the MOTD (if any, as per
	the MOTD reply. (RFC2813)
*/

std::string		reply_welcome(IRCserv *serv, Client *cli)
{
	std::string	reply = "";
	std::string	nick = cli->getnick();
	std::string	user = cli->getusername();
	std::string	host = cli->gethostname();

	reply += ft_buildmsg(serv->servername, RPL_WELCOME,
		nick, "", "Welcome to the Internet Relay Network " + nick + "!" +
		user + "@" + host);
	reply += ft_buildmsg(serv->servername, RPL_YOURHOST, nick, "",
		"Your host is " + serv->servername + ", running version " + VERSIONSTRING);
	reply += ft_buildmsg(serv->servername, RPL_CREATED, nick, "",
		"This server was created " + ft_timetostring(serv->dtstarted));
	reply += ft_buildmsg(serv->servername, RPL_MYINFO, nick,
		serv->servername + " " + VERSIONSTRING + " " +
		serv->usermodes + " " + serv->chanmodes, "");
	reply += reply_lusers(serv, nick);
	reply += reply_motd(serv, nick);
	return (reply);
}

std::string	reply_chan_names(IRCserv *serv, std::list<Channel>::iterator chan, Client *client)
{
	std::string	reply;
	std::unordered_map<Client*, client_flags>::iterator c_map;

	reply += ":" + serv->servername + " " + RPL_NAMREPLY + " " +
		client->getnick() + " ";
	if (chan->isSecret())
		reply += "@ ";
	else if (chan->isPrivate())
		reply += "* ";
	else
		reply += "= ";
	reply += chan->getname() + " :";
	for (c_map = chan->getclients().begin(); c_map != chan->getclients().end(); c_map++)
	{
		if (client->isOperator() || !c_map->first->isInvisible())
		{
			if (c_map->second._operator)
				reply += "@";
			reply += c_map->first->getnick() + " ";
		}
	}
	reply.pop_back();
	reply += CRLF;
	reply += ":" + serv->servername + " 366 " + client->getnick() + " " +
		chan->getname() + " " + ":End of /NAMES list." + CRLF;
	return reply;
}

std::string	reply_nochan_visible_names(IRCserv *serv, Client *client)
{
	std::string reply;
	std::list<Client>::iterator	client_it;
	std::list<Channel*>::iterator chan;
	std::vector<t_server>::iterator	net;
	size_t	counter;

	counter = 0;
	reply += ":" + serv->servername + " " + RPL_NAMREPLY + " " + client->getnick();
	reply += " * :";
	for (client_it = serv->clients.begin(); client_it != serv->clients.end(); client_it++)
	{
		chan = client_it->getchannels().begin();
		for (;chan != client_it->getchannels().end(); chan++)
			if (!((*chan)->getflags()._secret || (*chan)->getflags()._anonymous))
				break ;
		if ((client_it->getchannels().size() == 0 && (!client_it->isInvisible() ||
						client->isOperator())) ||
				((chan == client_it->getchannels().end() &&
				 client_it->getchannels().size() > 0) && (!client_it->isInvisible() ||
				client->isOperator())))
		{
			if (client_it->isOperator())
				reply += "@";
			reply += client_it->getnick() + " ";
			counter++;
		}
	}
	for (net = serv->network.begin(); net != serv->network.end(); net++)
	{
		client_it = net->clients.begin();
		for (; client_it != net->clients.end(); client_it++)
		{
			chan = client_it->getchannels().begin();
			for (;chan != client_it->getchannels().end(); chan++)
				if (!((*chan)->getflags()._secret || (*chan)->getflags()._anonymous))
					break ;
			if ((client_it->getchannels().size() == 0 && (!client_it->isInvisible() ||
							client->isOperator())) ||
				((chan == client_it->getchannels().end() &&
				  client_it->getchannels().size() > 0) && (!client_it->isInvisible() ||
															client->isOperator())))
			{
				if (client_it->isOperator())
					reply += "@";
				reply += client_it->getnick() + " ";
				counter++;
			}
		}
	}
	if (counter == 0)
		reply = "";
	else
	{
		reply.pop_back();
		reply += CRLF;
		reply += ":" + serv->servername + " 366 " + client->getnick() + " " +
			"*" + " " + ":End of /NAMES list." + CRLF;
	}
	return reply;

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

std::string		getservernamebymask(IRCserv *serv, std::string const &mask)
{
	std::string	servername = "";

	if (match(serv->servername, mask))
		servername = serv->servername;
	for (t_netit servit = serv->network.begin();
		servit != serv->network.end() && servername.empty(); servit++)
	{
		if (match(servit->servername, mask))
		{
			servername = servit->servername;
			break ;
		}
	}
	return (servername);
}

int				getserverfdbymask(IRCserv *serv, std::string const &mask)
{
	for (t_netit servit = serv->network.begin();
		servit != serv->network.end(); servit++)
	{
		if (match(servit->servername, mask))
			return (servit->fd);
	}
	return (-1);
}

std::string		getnicktoreply(int fd, const t_strvect &split, IRCserv *serv)
{
	t_citer it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (it != serv->clients.end())
		return (it->getnick());
	else if (split[0][0] == ':')
		return (split[0].substr(1));

	return ("");
}

size_t		getusercount(IRCserv *serv, std::string const &mask = "*",
	bool opersonly = false)
{
	size_t	count = 0;

	if (match(serv->servername, mask))
		for (t_citer cit = serv->clients.begin(); cit != serv->clients.end(); cit++)
			if (!opersonly || (opersonly && cit->isOperator()))
				count++;
	for (t_netit sit = serv->network.begin(); sit != serv->network.end(); sit++)
		if (match(sit->servername, mask))
			for (t_citer cit = sit->clients.begin(); cit != sit->clients.end(); cit++)
				if (!opersonly || (opersonly && cit->isOperator()))
					count++;
	return (count);
}

size_t		getservcount(IRCserv *serv, std::string const &mask = "*",
	bool connectedtomeonly = false)
{
	size_t	count = 0;

	if (!connectedtomeonly && match(serv->servername, mask))
		count++;
	for (t_netit sit = serv->network.begin(); sit != serv->network.end(); sit++)
		if ((!connectedtomeonly || sit->hopcount == 1) && match(sit->servername, mask))
			count++;
	return (count);
}

size_t		getunknownconnectscount(IRCserv *serv)
{
	size_t	unkncount = serv->fds.size() - serv->clients.size() - serv->network.size();
	for (std::map<int, t_fd>::iterator fit = serv->fds.begin(); fit != serv->fds.end(); fit++)
		if (fit->second.type == FD_ME)
			unkncount--;
	return (unkncount);
}

std::string	reply_lusers(IRCserv *serv, std::string const &target, std::string const &mask)
{
	std::string	reply = "";
	size_t	usercount = getusercount(serv, mask);
	size_t	botscount = 0;		// services
	size_t	servcount = getservcount(serv, mask);
	size_t	opercount = getusercount(serv, mask, true);
	size_t	unkncount = getunknownconnectscount(serv);
	size_t	chancount = serv->channels.size();
	size_t	svmecount = getservcount(serv, mask, true);
	size_t	clmecount = 0;
	if (match(serv->servername, mask))
		clmecount = serv->clients.size();

//  MUST send back RPL_LUSERCLIENT and RPL_LUSERME. others only when non-zero

	reply += ft_buildmsg(serv->servername, RPL_LUSERCLIENT, target, "", "There are " +
		std::to_string(usercount) + " users and " +
		std::to_string(botscount) + " services on " +
		std::to_string(servcount) + " servers");

	if (opercount != 0)
		reply += ft_buildmsg(serv->servername, RPL_LUSEROP, target,
			std::to_string(opercount), "operator(s) online");
	if (unkncount != 0)
		reply += ft_buildmsg(serv->servername, RPL_LUSERUNKNOWN, target,
			std::to_string(unkncount), "unknown connection(s)");
	if (chancount != 0)
		reply += ft_buildmsg(serv->servername, RPL_LUSERCHANNELS, target,
			std::to_string(chancount), "channels formed");

	reply += ft_buildmsg(serv->servername, RPL_LUSERME, target, "", "I have " +
		std::to_string(clmecount) + " clients and " +
		std::to_string(svmecount) + " servers");

	return (reply);
}
