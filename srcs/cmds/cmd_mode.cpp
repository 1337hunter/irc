/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_mode.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/10 19:41:55 by gbright           #+#    #+#             */
/*   Updated: 2020/12/19 21:53:29 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "message.hpp"
#include "tools.hpp"
#include "commands.hpp"

void	mode_from_network(int fd, const t_strvect &split, IRCserv *serv)
{
	int		ret = 0;
	Channel	*channel_mode;
	Client	*client_mode;
	std::string	temp;

	if (split.size() < 4 || split[2][0] == '+')
		return ; // error from other server (may be msg_error here)
	if (split[3][0] == ':')
		temp = std::string(split[3], 1);
	else
		temp = split[3];
	temp += strvect_to_string(split, ' ', 4);
	if (split[2][0] == '#' || split[2][0] == '!')
	{
		if ((channel_mode = find_channel_by_name(split[2], serv)) == 0)
			return ;
		if (split.size() == 4)
			ret = channel_mode->setMode((split[3][0] == ':' ? std::string(split[3], 1) :
						split[3]));
		else
			ret = channel_mode->setMode(ft_splitstring(temp, ' '));
		if (ret == 461 || ret == INT_ERR_KEYSET || ret == 1)
			return ;
	}
	else
	{
		if ((client_mode = find_client_by_nick(split[2], serv)) == 0)
			return ;
		client_mode->setUMODE((split[3][0] == ':' ? std::string(split[3], 1) : split[3]));
	}
	msg_forward(fd, strvect_to_string(split), serv);
}

void	mode_from_client(int fd, const t_strvect &split, IRCserv *serv)
{
	Client	*client;
	Channel	*channel_mode;
	Client	*client_mode;
	size_t	pos;
	int		ret;

	if (!(client = find_client_by_fd(fd, serv)) || !client->isRegistred()) {
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1, "",
				"You have not registered"); return ;
	}
	if (split.size() < 2) {
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, fd, "MODE",
				"Not enough parameters"); return ;
	}
	if (split[1][0] == '+')
	{
		serv->fds[fd].wrbuf += get_reply(serv, "477", fd, split[1],
		"Channel doesn't support modes"); return ;
	}
	if (split[1][0] == '#' || split[1][0] == '!' || split[1][0] == '&')
	{
		if (!(channel_mode = find_channel_by_name(split[1], serv)))
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHCHANNEL, fd, split[1],
					"No such channel"); return ;
		}
		if (split.size() == 2)
		{
			serv->fds[fd].wrbuf += ":" + serv->servername + " 324 ";
			serv->fds[fd].wrbuf += client->getnick() + " " + split[1] + " ";
			serv->fds[fd].wrbuf += channel_mode->getMode() + CRLF;
			serv->fds[fd].wrbuf += ":" + serv->servername + " 329 " +
				client->getnick() + " " + split[1] + " " +
				channel_mode->getCreationTime() + CRLF;
			return ;
		}
		if (!channel_mode->isOnChan(client))
		{
			serv->fds[fd].wrbuf += get_reply(serv, "442", fd, split[1],
					"You're not on that channel"); return ;
		}
		if (split.size() == 3 && split[2][0] != '-' && split[2][0] != '+')
		{

			if ((pos = split[2].find_first_not_of("eIOb")) != std::string::npos)
			{
				serv->fds[fd].wrbuf += get_reply(serv, ERR_UNKNOWNMODE, fd,
				std::string(1, split[2][pos]), "is unknown mode char to me for " + split[1]);
				return ;
			}
			serv->fds[fd].wrbuf += get_mask_reply(channel_mode, client, split[2], serv);
			return ;
		}
		if (!channel_mode->isOperator(client))
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_CHANOPRIVSNEEDED, fd, split[1],
					"You're not channel operator"); return ;
		}
		for (size_t j = 2; j < split.size(); j++)
			if (split[j].size() > 0 && (split[j][0] == '+' || split[j][0] == '-'))
			{
				if ((pos = split[j].find_first_not_of("+-OovaimnqpsrtklbeI")) != NPOS)
				{
					serv->fds[fd].wrbuf += get_reply(serv, ERR_UNKNOWNMODE, fd,
					std::string(1, split[j][pos]), "is unknown mode char to me for " +
					split[1]);
					return ;
				}
			}
		if (split.size() == 3)
			ret = channel_mode->setMode(split[2]);
		else
		{
			ret = channel_mode->setMode(ft_splitstring(
					strvect_to_string(split, ' ', 2), ' '));
		}
		if (ret == INT_ERR_NEEDMOREPARAMS)
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, fd, "MODE",
					"Not enough parameters"); return ;
		}
		else if (ret == INT_ERR_KEYSET)
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_KEYSET, fd, split[1],
					"Channel key already set"); return ;
		}
		else if (ret > 0)
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_KEYSET, fd, split[1],
					"No such nick/channel"); return ;
		}
	}
	else
	{
		client_mode = find_client_by_nick(split[1], serv);
		if (client_mode == 0 || client_mode->getnick() != client->getnick())
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_USERSDONTMATCH, fd, "",
					"Cannot change mode for other users"); return ;
		}
		if (split.size() == 2)
		{
			serv->fds[fd].wrbuf += ":" + serv->servername + " 221 " +
				client->getnick() + " " + client->getMode() + CRLF; return ;
		}
		if (split.size() == 3)
		{
			if (split[2].find_first_not_of("aiwrOos+-") != NPOS)
			{
				serv->fds[fd].wrbuf += get_reply(serv, ERR_UMODEUNKNOWNFLAG, fd, "",
						"Unknown MODE flag"); return ;
			}
			if (split[2][0] != '+' && split[2][0] != '-')
			{
				serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, fd, "MODE",
						"Not enough parameters"); return ;
			}
			client->setUMODE(split[2]);
		}
	}
	if (split[1][0] != '&' && split.size() > 2)
		msg_forward(fd, ":" + client->getinfo() + " " + strvect_to_string(split), serv);
	serv->fds[fd].wrbuf += ":" + client->getsafeinfo() + " " +
		strvect_to_string(split) + CRLF;
}

void	cmd_mode(int fd, const t_strvect &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_SERVER)
		mode_from_network(fd, split, serv);
	else
		mode_from_client(fd, split, serv);
}
