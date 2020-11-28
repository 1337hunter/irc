/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/26 21:08:41 by salec             #+#    #+#             */
/*   Updated: 2020/11/28 20:03:08 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ctime>
#include <sys/stat.h>
#include "ircserv.hpp"
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

t_strvect	ft_splitstring(std::string msg, std::string const &delim)
{
	std::vector<std::string>	split;
	size_t						pos = 0;
	std::string					token;
	std::string					reply;
	std::string					tmp;

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
	std::vector<std::string>	split;
	size_t						pos = 0;
	std::string					token;
	std::string					reply;
	std::string					tmp;

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

std::string	strvect_to_string(const t_strvect &split, char delimiter, size_t position, size_t len)
{
	std::string	output_string;
	size_t		i;

	i = position - 1;
	while (++i < split.size() && len)
	{
		output_string += split[i];
		if (i + 1 < split.size() && i + 1 < len)
			output_string += delimiter;
		len--;
	}
	return output_string;
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
	if (!msg.empty() || msgcode != RPL_MOTD || msgcode != RPL_INFO)
		res += " :" + msg;
	res += CRLF;
	return (res);
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

typedef struct stat		t_stat;

std::string	ft_gettimecompiledstring(void)
{
	int			fd = open("./ircserv", O_RDONLY);
	std::string	res = "";

	if (fd > 0)
	{
		t_stat	stat;
		if (fstat(fd, &stat) == 0)
		{
			time_t		rawtime = stat.st_mtim.tv_sec;
			struct tm	*timeinfo = localtime(&rawtime);

			if (timeinfo->tm_mday < 10)
				res += "0";
			res += std::to_string(timeinfo->tm_mday) + "/";
			if (timeinfo->tm_mon + 1 < 10)
				res += "0";
			res += std::to_string(timeinfo->tm_mon + 1) + "/";
			res += std::to_string(timeinfo->tm_year + 1900) + " ";

			if (timeinfo->tm_hour < 10)
				res += "0";
			res += std::to_string(timeinfo->tm_hour) + ":";
			if (timeinfo->tm_min < 10)
				res += "0";
			res += std::to_string(timeinfo->tm_min) + ":";
			if (timeinfo->tm_sec < 10)
				res += "0";
			res += std::to_string(timeinfo->tm_sec);
			// tm_zone is GNU/BSD extension
			# ifdef	__USE_MISC
				res += + " ";
				res += (timeinfo->tm_zone);
			# endif
		}
		close(fd);
	}
	return (res);
}

std::string	ft_gettimestring(void)
{
	time_t			rawtime;
	struct tm		*timeinfo;
	std::string		res = "";

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	if (timeinfo->tm_mday < 10)
		res += "0";
	res += std::to_string(timeinfo->tm_mday) + "/";
	if (timeinfo->tm_mon + 1 < 10)
		res += "0";
	res += std::to_string(timeinfo->tm_mon + 1) + "/";
	res += std::to_string(timeinfo->tm_year + 1900) + " ";

	if (timeinfo->tm_hour < 10)
		res += "0";
	res += std::to_string(timeinfo->tm_hour) + ":";
	if (timeinfo->tm_min < 10)
		res += "0";
	res += std::to_string(timeinfo->tm_min) + ":";
	if (timeinfo->tm_sec < 10)
		res += "0";
	res += std::to_string(timeinfo->tm_sec);
	// tm_zone is GNU/BSD extension
	# ifdef	__USE_MISC
		res += + " ";
		res += (timeinfo->tm_zone);
	# endif
	return (res);
}

Client	*find_client_by_nick(std::string const &nick, IRCserv *serv)
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

t_server	*find_server_by_fd(int fd, IRCserv *serv)
{
	std::vector<t_server>::iterator	net = serv->network.begin();

	while (net != serv->network.begin())
	{
		if (fd == net->fd)
			return &(*net);
		net++;
	}
	return 0;
}

/*int		find_client_routing(std::string	const &nick, IRCserv *serv)
{
	std::list<Client>::iterator     client = serv->clients.begin();
	std::vector<t_server>::iterator net = serv->network.begin();

	while (client != serv->clients.end())
	{
		if (client->getnickname() == nick)
			return client->getFD();
		client++;
	}
	while (net != serv->network.end())
	{
		client = net->clients.begin();
		while (client != net->clients.end())
		{
			if (client->getnickname() == nick)

		}
	}
}*/
