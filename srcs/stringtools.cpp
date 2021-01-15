/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stringtools.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/05 00:41:06 by salec             #+#    #+#             */
/*   Updated: 2021/01/15 16:26:40 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tools.hpp"
#include <sstream>

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

t_strvect	ft_splitcmdbyspace(std::string msg)
{
	t_strvect		split;
	size_t			pos = 0;
	std::string		tmp;

	while ((pos = msg.find_first_of(" ")) != std::string::npos)
	{
		tmp = msg.substr(0, pos);
		if (!tmp.empty())
			split.push_back(tmp);
		msg.erase(0, pos + 1);
		if (!msg.empty() && msg[0] == ':')
			break ;
	}
	if (!msg.empty())
		split.push_back(msg);

	// V this is a part of a stupid fix for privmsg
	size_t	i = 0;
	if (split.size() > 0 && split[0][0] == ':')
		i = 1;
	// ^ this is a part of a stupid fix for privmsg

	if (split.back().size() > 0 && split.back()[0] == ':' &&
		(ft_strtoupper(split[i]) != "PRIVMSG") &&
		(ft_strtoupper(split[i]) != "NOTICE"))	// this is the stupid fix
		split.back() = split.back().substr(1);
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

std::string	ft_strtolower(std::string const &str)
{
	std::string	res = str;

	for (int i = 0; res[i] != '\0'; i++)
	{
		if (res[i] >= 'A' && res[i] <= 'Z')
			res[i] = std::tolower(res[i]);
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
				reply += ":" + serv->servername + " 367 " + client->getnick();
				reply += " " + channel->getname() + " " + *mask + CRLF;
				mask++;
			}
			reply += ":" + serv->servername + " 368 " + client->getnick();
			reply += " " + channel->getname() + " :End of channel ban list" + CRLF;
		}
		else if (mode[i] == 'I')
		{
			mask = channel->getflags()._Invitation_mask.begin();
			while (mask != channel->getflags()._Invitation_mask.end())
			{
				reply += ":" + serv->servername + " 346 " + client->getnick();
				reply += " " + channel->getname() + " " + *mask + CRLF;
				mask++;
			}
			reply += ":" + serv->servername + " 347 " + client->getnick();
			reply += " " + channel->getname() + " :End of channel invite list" + CRLF;
		}
		else if (mode[i] == 'e')
		{
			mask = channel->getflags()._exception_mask.begin();
			while (mask != channel->getflags()._exception_mask.end())
			{
				reply += ":" + serv->servername + " 348 " + client->getnick();
				reply += " " + channel->getname() + " " + *mask + CRLF;
				mask++;
			}
			reply += ":" + serv->servername + " 349 " + client->getnick();
			reply += " " + channel->getname() + " :End of channel exception list" + CRLF;
		}
		else if (mode[i] == 'O')
		{
			reply += ":" + serv->servername + " 325 " + client->getnick();
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
	if (*it != '#' && *it != '$')
		return false;
	rit = mask.rbegin();
	while (rit != mask.rend() && *rit != '.') rit++;
	if (rit == mask.rend()) return false;
	while (rit != mask.rbegin() && *rit != '*') rit--;
	if (*rit == '*') return false;
	return true;
}

std::string		ft_tostring(int val)
{
	std::ostringstream	oss;
	oss << val;
	return (oss.str());
}

std::string 	ft_tostring(long val)
{
	std::ostringstream	oss;
	oss << val;
	return (oss.str());
}

std::string 	ft_tostring(uint val)
{
	std::ostringstream	oss;
	oss << val;
	return (oss.str());
}

std::string 	ft_tostring(ulong val)
{
	std::ostringstream	oss;
	oss << val;
	return (oss.str());
}

int				ft_stoi(std::string const &str)
{
	std::istringstream	iss(str);
	int					val = 0;

	if (!(iss >> val))
		return (0);
	return (val);
}

long			ft_stol(std::string const &str)
{
	std::istringstream	iss(str);
	long				val = 0;

	if (!(iss >> val))
		return (0L);
	return (val);
}

uint			ft_stou(std::string const &str)
{
	std::istringstream	iss(str);
	unsigned int		val = 0;

	if (!(iss >> val))
		return (0U);
	return (val);
}

ulong			ft_stoul(std::string const &str)
{
	std::istringstream	iss(str);
	unsigned long		val = 0;

	if (!(iss >> val))
		return (0UL);
	return (val);
}
