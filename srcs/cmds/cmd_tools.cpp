/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_tools.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/11 17:08:35 by gbright           #+#    #+#             */
/*   Updated: 2020/12/07 17:14:51 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
			"", serv->servername + " Message of the day");
		while (!motd.eof())
		{
			std::getline(motd, motdstr);
			if (motd.eof() && motdstr.length() == 0)
				break ;
			if (motdstr.length() > 80)
				motdstr = motdstr.substr(0, 80);
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

std::string		reply_welcome(IRCserv *serv, t_citer const cli)
{
	std::string	reply = "";
	std::string	nick = cli->getnickname();
	std::string	user = cli->getusername();
	std::string	host = cli->gethostname();

	reply += ft_buildmsg(serv->servername, RPL_WELCOME,
		nick, "", "Welcome to the Internet Relay Network " + nick + "!" +
		user + "@" + host);
	reply += ft_buildmsg(serv->servername, RPL_YOURHOST, nick, "",
		"Your host is " + serv->servername + ", running version " +
		serv->version);
	reply += ft_buildmsg(serv->servername, RPL_CREATED, nick, "",
		"This server was created " + ft_timetostring(serv->dtstarted));
	reply += ft_buildmsg(serv->servername, RPL_MYINFO, nick,
		serv->servername + " " + serv->version + " " +
		serv->usermodes + " " + serv->chanmodes, "");
	reply += reply_motd(serv, nick);
	return (reply);
}

std::string	reply_chan_names(IRCserv *serv, std::list<Channel>::iterator chan, Client *client)
{
	std::string	reply;
	std::unordered_map<Client*, client_flags>::iterator c_map;

	reply += ":" + serv->servername + " " + RPL_NAMREPLY + " " +
		client->getnickname() + " ";
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
			reply += c_map->first->getnickname() + " ";
		}
	}
	reply.pop_back();
	reply += CRLF;
	reply += ":" + serv->servername + " 366 " + client->getnickname() + " " +
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
	reply += ":" + serv->servername + " " + RPL_NAMREPLY + " " + client->getnickname();
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
			reply += client_it->getnickname() + " ";
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
				reply += client_it->getnickname() + " ";
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
		reply += ":" + serv->servername + " 366 " + client->getnickname() + " " +
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
