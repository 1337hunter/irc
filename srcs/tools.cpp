/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/26 21:08:41 by salec             #+#    #+#             */
/*   Updated: 2020/12/21 16:50:37 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tools.hpp"

t_citer		ft_findclientfd(t_citer const &begin, t_citer const &end, int fd)
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
		if (it->getnick() == nick)
			return (it);
	return (end);
}

Client		*find_client_by_nick(std::string const &nick, IRCserv *serv)
{
	std::list<Client>::iterator		client = serv->clients.begin();
	std::vector<t_server>::iterator	net = serv->network.begin();

	while (client != serv->clients.end())
	{
		if (client->getnick() == nick)
			return &(*client);
		client++;
	}
	while (net != serv->network.end())
	{
		client = net->clients.begin();
		while (client != net->clients.end())
		{
			if (client->getnick() == nick)
				return &(*client);
			client++;
		}
		net++;
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

t_server	*find_server_by_token(std::string const &token, IRCserv *serv)
{
	std::vector<t_server>::iterator	net;

	net = serv->network.begin();
	for (; net != serv->network.end(); net++)
		if (net->token == token)
			return &(*net);
	return 0;
}

Client		*find_client_by_info(std::string const &info, IRCserv *serv)
{
	std::list<Client>::iterator		client = serv->clients.begin();
	std::vector<t_server>::iterator	net = serv->network.begin();

	while (client != serv->clients.end())
	{
		if (client->getinfo() == info)
			return &(*client);
		client++;
	}
	while (net != serv->network.end())
	{
		client = net->clients.begin();
		while (client != net->clients.end())
		{
			if (client->getinfo() == info)
				return &(*client);
			client++;
		}
	}
	return 0;
}

Client		*find_client_by_user_or_nick_and_host(std::string const &str, IRCserv *serv)
{
	t_strvect				user_and_host;
	t_strvect				host_and_server;
	std::list<Client>::iterator		client = serv->clients.begin();
	std::vector<t_server>::iterator	net = serv->network.begin();

	std::cout << str << "\n\n";
	if (str.find_first_of("!") != NPOS)
		return find_client_by_info(str, serv);
	user_and_host = ft_splitstring(str, "%");
	if (user_and_host.size() != 2)
		return 0;
	host_and_server = ft_splitstring(user_and_host[1], '@');
	if (host_and_server.size() == 1)
	{
		while (client != serv->clients.end())
		{
			if (client->getusername() == user_and_host[0] &&
					client->gethostname() == user_and_host[1])
				return &(*client);
			client++;
		}
	}
	else if (host_and_server.size() == 2)
	{
		if (serv->servername == host_and_server[1])
		{
			client = serv->clients.begin();
			for (; client != serv->clients.end(); client++)
			{
				if (client->getusername() == user_and_host[0] &&
					client->gethostname() == host_and_server[0])
					return &(*client);
			}
			return 0;
		}
		net = serv->network.begin();
		for (; net != serv->network.end(); net++)
			if (net->servername == host_and_server[1])
				break ;
		if (net == serv->network.end())
			return 0;
		else
		{
			client = net->clients.begin();
			for (; client != net->clients.end(); client++)
			{
				if (client->getusername() == user_and_host[0] &&
						client->gethostname() == host_and_server[0])
					return &(*client);
			}
		}
	}
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

t_server	*find_server_by_mask(std::string const &mask, IRCserv *serv)
{
	std::vector<t_server>::iterator net = serv->network.begin();
	std::list<t_server_intro>::iterator routing;

	for (; net != serv->network.end(); net++)
	{
		if (match(net->servername, mask))
			return &(*net);
		for (routing = net->routing.begin(); routing != net->routing.end(); ++routing)
			if (match(routing->servername, mask))
				return &(*net);
	}
	return 0;
}

t_server	*find_server_by_name(std::string const &name, IRCserv *serv)
{
	std::vector<t_server>::iterator net = serv->network.begin();
	std::list<t_server_intro>::iterator routing;

	for (; net != serv->network.end(); net++)
	{
		if (net->servername == name)
			return &(*net);
		for (routing = net->routing.begin(); routing != net->routing.end(); ++routing)
			if (routing->servername == name)
				return &(*net);
	}
	return 0;
}

std::string	ft_buildmsg(std::string const &from, std::string const &msgcode,
	std::string const &to, std::string const &cmd, std::string const &msg)
{
	std::string	res;

	res = ":" + from + " " + msgcode + " ";
	if (to.empty())
		res += "*";
	else
		res += to;
	if (!cmd.empty())
		res += " " + cmd;
	if (!msg.empty() || msgcode == RPL_MOTD || msgcode == RPL_INFO ||
		msgcode == RPL_ISON || msgcode == RPL_USERHOST)
		res += " :" + msg;
	res += CRLF;
	return (res);
}

void		addtonickhistory(IRCserv *serv, t_citer const client)
{
	t_whowas	entry;

	entry.nickname = client->getnick();
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

int			nick_forward(IRCserv *serv, Client *client)
{
	std::string	forward;
	std::vector<t_server>::iterator	net;

	if (!client)
		return 1;
	forward = "NICK " + client->getnick() + " " + std::to_string(client->gethop() + 1)
	   	+ " " + client->getusername() + " " + client->gethostname() + " " +
		client->gettoken() + " " + client->getMode() + " " +  ":" +
		client->getrealname() + CRLF;
	for (net = serv->network.begin(); net != serv->network.end(); net++)
		serv->fds[net->fd].wrbuf += forward;
	return 0;
}

bool	remove_channel(Channel *channel, IRCserv *serv)
{
	std::list<Channel>::iterator	chan;

	for (chan = serv->channels.begin(); chan != serv->channels.end(); chan++)
		if (channel == &(*chan))
		{
			serv->channels.erase(chan);
			return false;
		}
	return true;
}

bool	remove_client_by_ptr(Client *ptr, IRCserv *serv)
{
	std::vector<t_server>::iterator	net;
	std::list<Client>::iterator		client;

	client = serv->clients.begin();
	for (; client != serv->clients.end(); client++)
		if (&(*client) == ptr)
		{
			serv->clients.erase(client);
			return false;
		}
	net = serv->network.begin();
	for (; net != serv->network.end(); net++)
		for (client = net->clients.begin(); client != net->clients.end(); client++)
			if (&(*client) == ptr)
			{
				net->clients.erase(client);
				return false;
			}
	return true;
}

bool	remove_client_by_nick(std::string const &nick, IRCserv *serv)
{
	std::vector<t_server>::iterator	net;
	std::list<Client>::iterator		client;

	client = serv->clients.begin();
	for (; client != serv->clients.end(); client++)
		if (client->getnick() == nick)
		{
			serv->clients.erase(client);
			return false;
		}
	net = serv->network.begin();
	for (; net != serv->network.end(); net++)
		for (client = net->clients.begin(); client != net->clients.end(); client++)
			if (client->getnick() == nick)
			{
				net->clients.erase(client);
				return false;
			}
	return true;
}

void	clear_kill_list(IRCserv *serv)
{
	std::list<t_kill>::iterator	kill;

	kill = serv->kills.begin();
	while (kill != serv->kills.end())
		if (ft_getcurrenttime() - kill->time >= KILLTIME)
		{
			serv->kills.erase(kill);
			kill = serv->kills.begin();
		}
		else
			kill++;
}
