/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/09 18:57:17 by salec             #+#    #+#             */
/*   Updated: 2021/02/04 00:15:17 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bottools.hpp"
#include <stdlib.h>
#include <unistd.h>
#include <sstream>

void		error_exit(std::string const &s1)
{
	std::cerr << s1 << std::endl;
	exit(1);
}

void		ioerror(int sock, std::string const &msg)
{
	close(sock);
	error_exit(msg);
}

int			ft_stoi(std::string const &str)
{
	std::istringstream	iss(str);
	int					val = 0;

	if (!(iss >> val))
		return (0);
	return (val);
}

std::string	ft_tostring(int val)
{
	std::ostringstream	oss;
	oss << val;
	return (oss.str());
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

t_strvect	ft_splitcmdbyspace(std::string msg)
{
	t_strvect		split;
	size_t			pos = 0;
	std::string		tmp;

	if ((pos = msg.find_last_not_of(" \t\f\v\n\r")) != std::string::npos)
		msg.erase(pos + 1);
	else
		return (split);

	if ((pos = msg.find_first_not_of(" \t\f\v\n\r")) != std::string::npos)
		msg = msg.substr(pos);
	else
		return (split);

	while ((pos = msg.find_first_of(" \t\f\v\n\r")) != std::string::npos)
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
	if (split.back().size() > 0 && split.back()[0] == ':')
		split.back() = split.back().substr(1);
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
