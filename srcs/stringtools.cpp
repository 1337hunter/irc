/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stringtools.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/05 00:41:06 by salec             #+#    #+#             */
/*   Updated: 2020/12/11 22:01:38 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tools.hpp"

t_strvect	ft_splitstring(std::string msg, std::string const &delim)
{
	t_strvect		split;
	size_t			pos = 0;
	std::string		tmp;

	while ((pos = msg.find(delim)) != std::string::npos)
	{
		tmp = msg.substr(0, pos);
		if (!tmp.empty())
			split.push_back(tmp);
		msg.erase(0, pos + delim.length());
	}
	if (!msg.empty())
		split.push_back(msg);
	return (split);
}

t_strvect	ft_splitstring(std::string str, char delim)
{
	t_strvect	split;
	size_t		pos(0);
	std::string	temp;

	while ((pos = str.find(delim)) != std::string::npos)
	{
		temp = str.substr(0, pos);
		if (!temp.empty())
			split.push_back(temp);
		str.erase(0, pos + 1);
	}
	if (!str.empty())
		split.push_back(str);
	return split;
}


t_strvect	ft_splitstringbyany(std::string msg, std::string const &delim)
{
	t_strvect		split;
	size_t			pos = 0;
	std::string		tmp;

	while ((pos = msg.find_first_of(delim)) != std::string::npos)
	{
		tmp = msg.substr(0, pos);
		if (!tmp.empty())
			split.push_back(tmp);
		msg.erase(0, pos + 1);
	}
	if (!msg.empty())
		split.push_back(msg);
	return (split);
}

std::string	strvect_to_string(const t_strvect &split, char delim, size_t pos,
	size_t len)
{
	std::string	output_string;
	size_t		i;

	i = pos - 1;
	while (++i < split.size() && len)
	{
		output_string += split[i];
		if (i + 1 < split.size() && i + 1 < len)
			output_string += delim;
		len--;
	}
	return (output_string);
}

bool		match(const char *s1, const char *s2)
{
	if (*s1 != '\0' && *s2 == '*')
		return (match(s1 + 1, s2) || match(s1, s2 + 1));
	if (*s1 == '\0' && *s2 == '*')
		return (match(s1, s2 + 1));
	if ((*s1 == *s2 || *s2 == '?')  && *s1 != '\0' && *s2 != '\0')
		return (match(s1 + 1, s2 + 1));
	if (*s1 == *s2 && *s1 == '\0' && *s2 == '\0')
		return (true);
	return (false);
}

bool		match(std::string const &s1, std::string const &s2)
{
	if (s1[0] == '\0' && s2[0] == '*')
		return (match(s1, s2.substr(1)));
	if (s1[0] != '\0' && s2[0] == '*')
		return (match(s1.substr(1), s2) || match(s1, s2.substr(1)));
	if ((s1[0] == s2[0] || s2[0] == '?') && s1[0] != '\0' && s1[0] != '\0')
		return (match(s1.substr(1), s2.substr(1)));
	if (s1[0] == s2[0] && s1[0] == '\0' && s2[0] == '\0')
		return (true);
	return (false);
}

std::string	ft_strtoupper(std::string const &str)
{
	std::string	res = str;

	for (int i = 0; res[i] != '\0'; i++)
	{
		if (res[i] >= 'a' && res[i] <= 'z')
			res[i] = std::toupper(res[i]);
	}
	return (res);
}

std::string	get_nick_from_info(std::string const &info)
{
	size_t	pos = 0;
	std::string	nick;

	if (info.empty())
		return nick;
	if (info[pos] == ':')
		pos++;
	while (pos < info.size() && info[pos] != '!')
		nick += info[pos++];
	return nick;
}

std::string	get_mask_reply(Channel *channel, Client *client, std::string mode, IRCserv *serv)
{
	size_t	i;
	std::string reply;
	std::list<std::string>::const_iterator	mask;

	i = -1;
	while (++i < mode.size())
	{
		if (mode[i] == 'b')
		{
			mask = channel->getflags()._ban_mask.begin();
			while (mask != channel->getflags()._ban_mask.end())
			{
				reply += ":" + serv->servername + " 367 " + client->getnickname();	
				reply += " " + channel->getname() + " " + *mask + CRLF;
				mask++;
			}
			reply += ":" + serv->servername + " 368 " + client->getnickname();
			reply += " " + channel->getname() + " :End of channel ban list" + CRLF;
		}
		else if (mode[i] == 'I')
		{
			mask = channel->getflags()._Invitation_mask.begin();
			while (mask != channel->getflags()._Invitation_mask.end())
			{
				reply += ":" + serv->servername + " 346 " + client->getnickname();
				reply += " " + channel->getname() + " " + *mask + CRLF;
				mask++;
			}
			reply += ":" + serv->servername + " 347 " + client->getnickname();
			reply += " " + channel->getname() + " :End of channel invite list" + CRLF;
		}
		else if (mode[i] == 'e')
		{
			mask = channel->getflags()._exception_mask.begin();
			while (mask != channel->getflags()._exception_mask.end())
			{
				reply += ":" + serv->servername + " 348 " + client->getnickname();
				reply += " " + channel->getname() + " " + *mask + CRLF;
				mask++;
			}
			reply += ":" + serv->servername + " 349 " + client->getnickname();
			reply += " " + channel->getname() + " :End of channel exception list" + CRLF;
		}
		else if (mode[i] == 'O')
		{
			reply += ":" + serv->servername + " 325 " + client->getnickname();
			reply += " " + channel->getname() + " ";
			reply += get_nick_from_info(channel->getCreator()) + CRLF;
		}
	}
	return reply;
}

bool	is_valid_mask(std::string mask)
{
	std::string::iterator	it;

	it = mask.begin();
	if (mask.find_first_of("\0 \r\n\b,") != NPOS) return false;
	while (it != mask.end() && *it != '!') it++;
	if (it == mask.end()) return false;
	while (it != mask.end() && *it != '@') it++;
	if (it == mask.end()) return false;
	return true;
}

bool	is_valid_serv_host_mask(std::string mask)
{
	std::string::iterator   it;
	std::string::reverse_iterator rit;

	if (mask.empty())
		return false;
	it = mask.begin();
	if (*it != '#' && *it != '&' && *it != '!' && *it != '$')
		return false;
	rit = mask.rbegin();
	while (rit != mask.rend() && *rit != '.') rit++;
	if (rit == mask.rend()) return false;
	while (rit != mask.rbegin() && *rit != '*') rit--;
	if (*rit == '*') return false;
	return true;
}
