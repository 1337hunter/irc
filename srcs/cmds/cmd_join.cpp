/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_join.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbright <gbright@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/01 15:42:46 by gbright           #+#    #+#             */
/*   Updated: 2020/12/02 20:32:16 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"

//Command: JOIN
//Parameters: <channel>[ %x7 <modes> ] *( "," <channel>[ %x7 <modes> ] )
//
//Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"

void	join_forward(std::string &name, std::string &key, t_citer client_it, IRCserv *serv,
		std::string	append)
{
	std::vector<t_server>::iterator	net;

	for (net = serv->network.begin(); net != serv->network.end(); net++)
		serv->fds[net->fd].wrbuf += ":" + client_it->getnickname() + append +
		   " JOIN " + name + " " + key + CRLF;
}

void	join_backward(IRCserv *serv, std::list<Channel>::iterator chan, t_citer client_it)
{
	std::unordered_map<Client*, client_flags>::iterator client;

	serv->fds[client_it->getFD()].wrbuf += ":" + client_it->getnickname() + "!" +
		client_it->getusername() + "@" + client_it->gethostname() + " JOIN :" +
		chan->getname() + CRLF;
	serv->fds[client_it->getFD()].wrbuf += ":" + serv->servername + " " +
		RPL_NAMREPLY + " " + client_it->getnickname() + " ";
	if (chan->isSecret())
		serv->fds[client_it->getFD()].wrbuf += "@ ";
	else if (chan->isPrivate())
		serv->fds[client_it->getFD()].wrbuf += "* ";
	else
		serv->fds[client_it->getFD()].wrbuf += "= ";
	serv->fds[client_it->getFD()].wrbuf += chan->getname() + " ";
	for (client = chan->getclients().begin(); client != chan->getclients().end(); client++)
	{
		if (client->second._operator)
			serv->fds[client_it->getFD()].wrbuf += "@";
		serv->fds[client_it->getFD()].wrbuf += client->first->getnickname() + " ";
	}
	serv->fds[client_it->getFD()].wrbuf += CRLF;
	serv->fds[client_it->getFD()].wrbuf += ":" + serv->servername + " " + "366" + " " +
		client_it->getnickname() + " " + chan->getname() + " " +
		":End of /NAMES list." + CRLF;
}

void	join_to_hash_chan(int fd, const t_strvect &split, IRCserv *serv, t_citer client_it)
{
	std::list<Channel>::iterator	chan;
	std::vector<std::string>		args;
	std::vector<std::string>		keys;
	size_t	i;

	args = ft_splitstring(split[1], ",");
	if (split.size() > 2)
		keys = ft_splitstring(split[2], ",");
	i = keys.size() - 1;
	while (++i < args.size())
		keys.push_back("");
	for (i = 0; i < args.size(); i++)
	{
		for (chan = serv->channels.begin(); chan != serv->channels.end(); chan++)
			if (chan->getname() == args[i])
			{
				if (keys[i] == chan->getkey())
				{
					chan->add_client(&(*client_it));
					join_forward(args[i], keys[i], client_it, serv, "");
					join_backward(serv, chan, client_it);
				}
				else
					serv->fds[fd].wrbuf += get_reply(serv, ERR_BADCHANNELKEY, fd,
							args[i], "Cannot join channel (+k)");
				break ;
			}
		if (chan == serv->channels.end())
		{
			serv->channels.push_back(Channel(args[i], keys[i], &(*client_it)));
			join_forward(args[i], keys[i], client_it, serv, "");
			join_backward(serv, --serv->channels.end(), client_it);
		}
	}
}

void	cmd_join(int fd, t_strvect const &split, IRCserv *serv)
{
	t_citer	client_it;
	if (split.size() < 2)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, fd, "JOIN",
				"Not enough parameters");
		return ;
	}

	client_it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (split[0] == "JOIN" && serv->fds[fd].type == FD_CLIENT)
	{
		if (!client_it->isRegistred())
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1 , "JOIN",
					"You have not registered");
			return ;
		}
		if (split[1][0] == '#')
			join_to_hash_chan(fd, split, serv, client_it);
	}
	/*
	t_citer client_it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	serv->fds[fd].wrbuf += ":" + client_it->getnickname() + "!" +
		client_it->getusername() + "@" + client_it->gethostname() + " " + "JOIN " +
		":" + split[1] + CRLF;
	serv->fds[fd].wrbuf += ":" + serv->servername + " " + "MODE " + split[1] +
		" " + "+nt" + CRLF + ":" + serv->servername + " " + "353" + " " +
		client_it->getnickname() +  " " + "=" " " + split[1] + " " + ":" + "@" +
		client_it->getnickname() + " nick1 " + "nick2 " + CRLF;
		*/
}
