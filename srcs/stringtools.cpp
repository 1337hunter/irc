/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stringtools.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/05 00:41:06 by salec             #+#    #+#             */
/*   Updated: 2020/12/05 02:53:59 by salec            ###   ########.fr       */
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
	if (*s1 == *s2 && *s1 != '\0' && *s2 != '\0')
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
	if (s1[0] == s2[0] && s1[0] != '\0' && s1[0] != '\0')
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
