/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/26 21:08:41 by salec             #+#    #+#             */
/*   Updated: 2020/12/07 13:16:33 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tools.hpp"

t_citer		ft_findclientfd(t_citer const &begin, t_citer const &end,
				int fd)
{
	for (t_citer it = begin; it != end; it++)
		if (it->isConnected() && it->getFD() == fd)
			return (it);
	return (end);
}

t_citer		ft_findnick(t_citer const &begin, t_citer const &end,
				std::string const &nick)
{
	for (t_citer it = begin; it != end; it++)
		if (it->getnickname() == nick)
			return (it);
	return (end);
}

Client		*find_client_by_nick(std::string const &nick, IRCserv *serv)
{
	std::list<Client>::iterator		client = serv->clients.begin();
	std::vector<t_server>::iterator	net = serv->network.begin();

	while (client != serv->clients.end())
	{
		if (client->getnickname() == nick)
			return &(*client);
		client++;
	}
	while (net != serv->network.end())
	{
		client = net->clients.begin();
		while (client != net->clients.end())
		{
			if (client->getnickname() == nick)
				return &(*client);
			client++;
		}
	}
	return 0;
}

Client		*find_client_by_fd(int	fd, IRCserv *serv)
{
	std::list<Client>::iterator	client;

	for (client = serv->clients.begin(); client != serv->clients.end(); client++)
		if (client->getFD() == fd)
			return &(*client);
	return 0;
}

Channel		*find_channel_by_name(const std::string	&name, IRCserv *serv)
{
	std::list<Channel>::iterator	chan;

	for (chan = serv->channels.begin(); chan != serv->channels.end(); chan++)
		if (chan->getname() == name)
			return chan->getptr();
	return 0;
}

t_server	*find_server_by_fd(int fd, IRCserv *serv)
{
	std::vector<t_server>::iterator	net = serv->network.begin();

	while (net != serv->network.end())
	{
		if (fd == net->fd)
			return &(*net);
		net++;
	}
	return 0;
}

std::string	ft_buildmsg(std::string const &srv, std::string const &msgcode,
	std::string const &target, std::string const &cmd, std::string const &msg)
{
	std::string	res;

	res = ":" + srv + " " + msgcode + " ";
	if (target.empty())
		res += "*";
	else
		res += target;
	if (!cmd.empty())
		res += " " + cmd;
	if (!msg.empty() || msgcode == RPL_MOTD || msgcode == RPL_INFO)
		res += " :" + msg;
	res += CRLF;
	return (res);
}

void		addtonickhistory(IRCserv *serv, t_citer const client)
{
	t_whowas	entry;

	entry.nickname = client->getnickname();
	entry.username = client->getusername();
	entry.hostname = client->gethostname();
	entry.realname = client->getrealname();
	entry.dtloggedin = client->gettimeloggedin();
	entry.servername = serv->servername;
	serv->nickhistory.push_back(entry);
	if (DEBUG_MODE)
		std::cout << serv->nickhistory.back().nickname << " " <<
			serv->nickhistory.back().username << "@" <<
			serv->nickhistory.back().hostname << " (" <<
			serv->nickhistory.back().realname << ") from " <<
			serv->nickhistory.back().servername << " (last login " <<
			ft_timetostring(serv->nickhistory.back().dtloggedin) <<
			") has been added to whowas history" << std::endl;
}

int			nick_forward(IRCserv *serv, t_citer client)
{
	std::string	forward;
	std::vector<t_server>::iterator	net;

	if (client == serv->clients.end())
		return 1;
	forward = "NICK " + client->getnickname() + " " + client->gethopcount(true, true) +
		client->getusername() + " " + client->gethostname() + " " + client->gettoken() +
		client->getMode(true) + ":" + client->getrealname() + CRLF;
	for (net = serv->network.begin(); net != serv->network.end(); net++)
		serv->fds[net->fd].wrbuf += forward;
	return 0;
}
