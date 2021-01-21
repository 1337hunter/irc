#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"

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

std::string	reply_chan_names(IRCserv *serv, Channel* chan, Client *client)
{
	std::string	reply;
	std::MAP<Client*, client_flags>::iterator c_map;

	reply = ":" + serv->servername + " " + RPL_NAMREPLY + " " +
		client->getnick() + " ";
	if (chan->isSecret())
		reply += "@ ";
	else if (chan->isPrivate())
		reply += "* ";
	else
		reply += "= ";
	reply += chan->getname() + " :";
	std::cout << "1" << std::endl;
	for (c_map = chan->getclients().begin(); c_map != chan->getclients().end(); c_map++)
	{
		std::cout << "c_map->first->getnick(): " << c_map->first->getnick() << std::endl;
		if (client->isOperator() || !c_map->first->isInvisible())
		{
			if (c_map->second._operator)
				reply += "@";
			reply += c_map->first->getnick();
			reply += ' ';
		}
	}
	reply = reply.substr(0, reply.size() - 1);
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
		if (chan == client_it->getchannels().end() && (!client_it->isInvisible() ||
						client->isOperator()))
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
			if (chan == client_it->getchannels().end() && (!client_it->isInvisible() ||
							client->isOperator()))
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
		reply = reply.substr(0, reply.size() - 1);
		reply += CRLF;
		reply += ":" + serv->servername + " 366 " + client->getnick() + " " +
			"*" + " " + ":End of /NAMES list." + CRLF;
	}
	return reply;

}

bool	is_server_registred(const std::string &name, std::string const token, IRCserv *serv)
{
	std::vector<t_server>::iterator nearest = serv->network.begin();
	std::list<t_server_intro>::iterator server_intro;
	std::vector<t_server>::iterator end = serv->network.end();
	std::string	except = TOSTRING(NPOS);

	while (nearest != end)
	{
		if (nearest->servername == name || (nearest->token == token &&
					nearest->token != except))
			return true;
		server_intro = nearest->routing.begin();
		while (server_intro != nearest->routing.end())
		{
			if (server_intro->servername == name || server_intro->behind == name ||
					(server_intro->token == token && server_intro->token != except))
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
	else
	{
		t_server	*remote = find_server_by_mask(mask, serv);
		if (remote)
			servername = remote->servername;
	}
	return (servername);
}

int				getserverfdbymask(IRCserv *serv, std::string const &mask)
{
	t_server	*remote = find_server_by_mask(mask, serv);
	if (remote)
		return (remote->fd);
	return (-1);
}

std::string		getnicktoreply(int fd, const t_strvect &split, IRCserv *serv)
{
	if (split[0][0] == ':')
		return (split[0].substr(1));
	t_citer it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (it != serv->clients.end())
		return (it->getnick());
	return ("");
}

std::string		reply_unknowncmd(int fd, const t_strvect &split, IRCserv *serv)
{
	size_t		i = 0;
	std::string	nick = "";
	t_citer	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (it != serv->clients.end() && it->isRegistred())
		nick = it->getnick();
	else if (split[0][0] == ':')
	{
		i = 1;
		nick = split[0].substr(1);
	}
	else
		return ("");
	std::string	cmd;
	if (i < split.size())
		cmd = split[i];
	return (ft_buildmsg(serv->servername, ERR_UNKNOWNCOMMAND, nick,
		cmd, "Unknown command"));
}
