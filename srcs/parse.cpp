/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 15:56:53 by gbright           #+#    #+#             */
/*   Updated: 2021/01/09 18:53:36 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * There is a lot of magic in this place!
*/

#include "error_handle.hpp"
#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"
#include <string>
#include <iostream>
#include <fstream>

#define LISTEN	0
#define LINK	1
#define ADMIN	2
#define OPER	3
#define ME		4
#define MOTD	5
#define SERVICE	6
#define NPOS	std::string::npos

typedef	int (*t_block)(std::ifstream &config, std::string &line, IRCserv *serv, size_t &line_number);
typedef std::MAP<int, t_block> t_blockmap;

size_t	ft_strlen(const char *s)
{
	size_t	c = 0;
	if (!s) return 0;
	while (s[c]) c++;
	return c;
}

size_t	get_qarg(size_t pos, std::string &line, std::string &dst, std::string const &argname)
{
	size_t i;
	pos = line.find_first_not_of(" \t", pos + argname.size());
	if (pos == NPOS || line[pos] != '"')
		return NPOS;
	if ((i = line.find("\"", pos + 1)) == NPOS)
		return NPOS;
	dst = std::string(line, pos + 1, i - pos - 1);
	pos = line.find_first_not_of(" \t", i + 1);
	if (pos == NPOS || line[pos] != ';')
		return NPOS;
	return pos;
}

size_t	get_arg(size_t pos, std::string &line, std::string &dst, std::string const &argname)
{
	size_t	i;
	size_t	pos_copy_from;
	size_t	pos_copy_size;

	pos = line.find_first_not_of(" \t", pos + argname.size());
	if (pos == NPOS)
		return NPOS;
	i = line.find_first_of(" \t;", pos);
	pos_copy_from = pos;
	pos_copy_size = i - pos;
	if (i == NPOS)
		return NPOS;
	pos = line.find_first_not_of(" \t", i);
	if (pos == NPOS || line[pos] != ';')
		return NPOS;
	dst = std::string(line, pos_copy_from, pos_copy_size);
	return pos;
}

int
block_listen(std::ifstream &config, std::string &line, IRCserv *serv, size_t &line_number)
{
	size_t		pos;
	size_t		pos_copy_from;
	size_t		pos_copy_size;
	size_t		i;
	t_listen	temp;

	temp.ssl = false;
	temp.tls = false;
	temp.serveronly = false;
	pos = line.find_first_not_of(" \t");
	pos += ft_strlen("listen");
	while ((pos = line.find_first_not_of(" \t\n", pos)) == NPOS || line[pos] == '#')
	{
		getline(config, line);
		line_number++;
		if (config.eof())
			return -1;
		pos = 0;
	}
	if (pos >= line.length() || line[pos] != '{' || config.eof())
		return -1;
	pos++;
	while (pos >= line.length() || line[pos] != '}')
	{
		while ((pos = line.find_first_not_of(" \t\n", pos)) == NPOS)
		{
			getline(config, line);
			line_number++;
			if (config.eof())
				return -1;
			pos = 0;
		}
		if (!line.compare(pos, 2, "ip"))
		{
			pos = line.find_first_not_of(" \n\t", pos + 2);
			if (pos == NPOS)
				return -1;
			i = line.find_first_not_of("0123456789.", pos);
			if (i == NPOS)
				return -1;
			pos_copy_from = pos;
			if (line[i] == '*')
			{
				pos = line.find_first_not_of(" \n\t", i + 1);
				pos_copy_size = 1;
			}
			else
			{
				pos_copy_size = i - pos;
				pos = line.find_first_not_of(" \n\t", i);
			}
			if (pos == NPOS || line[pos] != ';')
				return -1;
			pos = line.find_first_not_of(" \t\n", pos + 1);
			if (pos != NPOS && line[pos] != '#' && line[pos] != '}'
					&& line.compare(pos, 4, "port") && line.compare(pos, 7, "options"))
				return -1;
			std::string	ip(line, pos_copy_from, pos_copy_size);
			temp.ip = ip;
		}
		else if (!line.compare(pos, 4, "port"))
		{
			pos = line.find_first_not_of(" \n\t", pos + 4);
			if (pos == NPOS)
				return -1;
			i = line.find_first_not_of("0123456789", pos);
			pos_copy_from = pos;
			pos_copy_size = i - pos;
			if (i == NPOS)
				return -1;
			pos = line.find_first_not_of(" \n\t", i);
			if (pos == NPOS || line[pos] != ';')
				return -1;
			pos = line.find_first_not_of(" \t\n", pos + 1);
			if (pos != NPOS && line[pos] != '#' && line[pos] != '}' &&
					line.compare(pos, 7, "options") && line.compare(pos, 2, "ip"))
				return -1;
			std::string	port(line, pos_copy_from, pos_copy_size);
			temp.port = STOI(port);
		}
		else if (!line.compare(pos, 7, "options"))
		{
			pos = line.find_first_not_of(" \n\t", pos + 7);
			if (pos == NPOS || line[pos] != '{')
				return -1;
			pos = line.find_first_not_of(" \n\t", pos + 1);
			while (pos != NPOS && line[pos] != '}')
			{
				if (!line.compare(pos, 3, "ssl"))
				{
					temp.ssl = true;
					if (temp.tls)
						return -1;
					pos = line.find_first_not_of(" \n\t", pos + 3);
					if (pos == NPOS || line[pos] != ';')
						return -1;
					pos++;
				}
				else if (!line.compare(pos, 10, "serveronly"))
				{
					temp.serveronly = true;
					pos = line.find_first_not_of(" \n\t", pos + 10);
					if (pos == NPOS || line[pos] != ';')
						return -1;
					pos++;
				}
				else if (!line.compare(pos, 3, "tls"))
				{
					temp.tls = true;
					if (temp.ssl)
						return -1;
					pos = line.find_first_not_of(" \n\t", pos + 3);
					if (pos == NPOS || line[pos] != ';')
						return -1;
					pos++;
				}
				else
					return -1;
				pos = line.find_first_not_of(" \n\t", pos);
			}
			if (pos == NPOS || line[pos] != '}')
				return -1;
			pos++;
		}
		else if (!line.compare(pos, 1, "#"))
		{
			pos = NPOS;
			continue ;
		}
		else if (!line.compare(pos, 1, "}"))
			break ;
		else
			return -1;
	}
	serv->listen.push_back(temp);
#if DEBUG_MODE
	std::vector<t_listen>::reverse_iterator	it = serv->listen.rbegin();
	std::cout << "listen: ip '" << it->ip << "' port '" << it->port << '\'' << ' ';
	if (temp.ssl)
		std::cout << "ssl ";
	if (temp.serveronly)
		std::cout << "serveronly ";
	if (temp.tls)
		std::cout << "tls ";
	std::cout << std::endl;
#endif
	return 0;
}

int
block_admin(std::ifstream &config, std::string &line, IRCserv *serv, size_t &line_number)
{
	size_t		pos;
	size_t		i;
	t_strvect	names;

	if (serv->admin.set)
		return -1;
	serv->admin.set = true;
	pos = line.find_first_not_of(" \t");
	pos += ft_strlen("admin");
	while ((pos = line.find("{", pos)) == NPOS)
	{
		getline(config, line);
		line_number++;
		if (config.eof())
			return -1;
		pos = 0;
	}
	if (line[pos] != '{' || config.eof())
		return -1;
	pos++;
	int k = 3;
	while (k)
	{
		if (pos < line.length() && line[pos] != '"')	// hmmmmmmm
			while ((pos = line.find_first_not_of(" \t\n", pos)) == NPOS)
			{
				getline(config, line);
				line_number++;
				if (config.eof())
					return -1;
				pos = 0;
			}
		if (line[pos] == '#' && (pos = NPOS))
			continue ;
		if (line[pos] != '"')
			return -1;
		if ((i = line.find("\"", pos + 1)) == NPOS)
			return -1;
		std::string	realname(line, pos + 1, i - pos - 1);
		names.push_back(realname);
		pos = line.find_first_not_of(" \t\n", i + 1);
		if (pos == NPOS || line[pos] != ';')
			return -1;
		pos = line.find_first_not_of(" \t\n", pos + 1);
		if (pos != NPOS && line[pos] != '#' && line[pos] != '"' &&
				!(k == 1 && line[pos] == '}'))
			return -1;
		if (pos < line.length() && line[pos] == '#')	// hmmmmmmmmm
			pos = NPOS;
		k--;
	}
	while ((pos = line.find_first_not_of(" \t\n", pos)) == NPOS)
	{
		getline(config, line);
		line_number++;
		if (config.eof())
			return -1;
		pos = 0;
	}
	if (line[pos] != '}')
		return -1;
	pos = line.find_first_not_of(" \t\n", pos + 1);
	if (pos != NPOS && line[pos] != '#')
		return -1;
	serv->admin.realname = names[0];
	serv->admin.nick = names[1];
	serv->admin.mail = names[2];
#if DEBUG_MODE
	std::cout << "admin.realname: \"" << serv->admin.realname << '"';
	std::cout << " admin.nick: \"" << serv->admin.nick << '"';
	std::cout << " admin.mail: \"" << serv->admin.mail << '"' << '\n';
#endif
	return 0;
}

int
block_link(std::ifstream &config, std::string &line, IRCserv *serv, size_t &line_number)
{
	t_link	temp;
	size_t	pos;
	size_t	i;
	size_t	pos_copy_from;
	size_t	pos_copy_size;

	temp.ssl = false;
	temp.tls = false;
	temp.autoconnect = false;
	pos = line.find_first_not_of(" \t");
	pos += ft_strlen("link");
	while ((pos = line.find_first_not_of(" \t", pos)) == NPOS || line[pos] == '#')
	{
		getline(config, line);
		line_number++;
		if (config.eof())
			return -1;
		pos = 0;
	}
	if (pos == NPOS || line[pos] != '{' || config.eof())
		return -1;
	pos++;
	while (pos >= line.length() || line[pos] != '}')
	{
		while ((pos = line.find_first_not_of(" \t", pos)) == NPOS)
		{
			getline(config, line);
			line_number++;
			if (config.eof())
				return -1;
			pos = 0;
		}
			if (!line.compare(pos, ft_strlen("servername"), "servername"))
		{
			pos = line.find_first_not_of(" \t", pos + ft_strlen("servername"));
			if (pos == NPOS)
				return -1;
			i = line.find_first_of(" \t;", pos);
			pos_copy_from = pos;
			pos_copy_size = i - pos;
			if (i == NPOS)
				return -1;
			pos = line.find_first_not_of(" \t", i);
			if (pos == NPOS || line[pos] != ';')
				return -1;
			pos = line.find_first_not_of(" \t", pos + 1);
			if (pos != NPOS && line[pos] != '#' && line[pos] != '}' && line.compare(pos, 8, "hostname") && line.compare(pos, 4, "pass") && line.compare(pos, 7, "options") && line.compare(pos, 4, "port"))
				return -1;
			std::string	what(line, pos_copy_from, pos_copy_size);
			temp.servername = what;
		}
		else if (!line.compare(pos, 8, "hostname"))
		{
			pos = line.find_first_not_of(" \t", pos + 8);
			if (pos == NPOS)
				return -1;
			i = line.find_first_of(" \t;", pos);
			pos_copy_from = pos;
			pos_copy_size = i - pos;
			if (i == NPOS)
				return -1;
			pos = line.find_first_not_of(" \t", i);
			if (pos == NPOS || line[pos] != ';')
				return -1;
			pos = line.find_first_not_of(" \t", pos + 1);
			 if (pos != NPOS && line[pos] != '#' && line[pos] != '}' && line.compare(pos, 2, "ip") && line.compare(pos, 4, "pass") && line.compare(pos, 7, "options") && line.compare(pos, 4, "port"))
				 return -1;
			 std::string     hostname(line, pos_copy_from, pos_copy_size);
			 temp.hostname = hostname;
		}
		else if (!line.compare(pos, 4, "port"))
		{
			pos = line.find_first_not_of(" \n\t", pos + 4);
			if (pos == NPOS)
				return -1;
			i = line.find_first_not_of("0123456789", pos);
			pos_copy_from = pos;
			pos_copy_size = i - pos;
			if (i == NPOS)
				return -1;
			pos = line.find_first_not_of(" \n\t", i);
			if (pos == NPOS || line[pos] != ';')
				return -1;
			pos = line.find_first_not_of(" \t\n", pos + 1);
			if (pos != NPOS && line[pos] != '#' && line[pos] != '}' && line.compare(pos, 2, "ip") && line.compare(pos, 4, "pass") && line.compare(pos, 7, "options") && line.compare(pos, 8, "hostname"))
				return -1;
			std::string     port(line, pos_copy_from, pos_copy_size);
			temp.port = STOI(port);
		}
		else if (!line.compare(pos, 4, "pass"))
		{
			pos = line.find_first_not_of(" \t", pos + 4);
			if (pos == NPOS || line[pos] != '"')
				return -1;
			if ((i = line.find("\"", pos + 1)) == NPOS)
				return -1;
			std::string	pass(line, pos + 1, i - pos - 1);
			temp.pass = pass;
			pos = line.find_first_not_of(" \t", i + 1);
			if (pos == NPOS || line[pos] != ';')
				return -1;
			pos = line.find_first_not_of(" \t", pos + 1);
			if (pos != NPOS && line[pos] != '#' && line[pos] != '}' && line.compare(pos, 2, "ip") && line.compare(pos, 4, "port") && line.compare(pos, 7, "options") && line.compare(pos, 8, "hostname"))
				return -1;
		}
		else if (!line.compare(pos, 7, "options"))
		{
			pos = line.find_first_not_of(" \n\t", pos + 7);
			if (pos == NPOS || line[pos] != '{')
				return -1;
			pos = line.find_first_not_of(" \n\t", pos + 1);
			while (pos != NPOS && line[pos] != '}')
			{
				if (!line.compare(pos, 3, "ssl"))
				{
					temp.ssl = true;
					if (temp.tls)
						return -1;
					pos = line.find_first_not_of(" \t", pos + 3);
					if (pos == NPOS || line[pos] != ';')
						return -1;
					pos++;
				}
				else if (!line.compare(pos, 3, "tls"))
				{
					temp.tls = true;
					if (temp.ssl)
						return -1;
					pos = line.find_first_not_of(" \t", pos + 3);
					if (pos == NPOS || line[pos] != ';')
						return -1;
					pos++;
				}
				else if (!line.compare(pos, 3, "tls"))
				{
					temp.tls = true;
					if (temp.ssl)
						return -1;
					pos = line.find_first_not_of(" \n\t", pos + 3);
					if (pos == NPOS || line[pos] != ';')
						return -1;
					pos++;
				}
				else
					return -1;
				pos = line.find_first_not_of(" \n\t", pos);
			}
			if (pos == NPOS || line[pos] != '}')
				return -1;
			pos++;
		}
		else if (!line.compare(pos, 1, "#"))
			pos = NPOS;
		else if (!line.compare(pos, 1, "}"))
			break ;
		else
			return -1;
	}
	serv->link.push_back(temp);
#if DEBUG_MODE
	std::vector<t_link>::reverse_iterator it = serv->link.rbegin();
	std::cout << "link: servername '" << it->servername << "' hostname '" << it->hostname << "'";
	std::cout << " port '" << it->port << "'" << " pass '" << it->pass << "' ";
	if (temp.ssl)
                std::cout << "'ssl' ";
        if (temp.autoconnect)
                std::cout << "'autoconnect' ";
        if (temp.tls)
				std::cout << "'tls' ";
		std::cout << std::endl;
#endif
	return 0;
}

int
block_me(std::ifstream &config, std::string &line, IRCserv *serv, size_t &line_number)
{
	size_t  pos;
	size_t  i;
	std::vector<std::string>		names;

	pos = line.find_first_not_of(" \t");
	pos += ft_strlen("me");
	while ((pos = line.find("{", pos)) == NPOS)
	{
		getline(config, line);
		line_number++;
		if (config.eof())
			return -1;
		pos = 0;
	}
	if (pos == NPOS || line[pos] != '{' || config.eof())
		return -1;
	pos++;
	int k = 3;
	while (k)
	{
		if (pos >= line.length() || line[pos] != '"')	// hmmmmmmmmm
			while ((pos = line.find_first_not_of(" \t\n", pos)) == NPOS)
			{
				getline(config, line);
				line_number++;
				if (config.eof())
					return -1;
				pos = 0;
			}
		if (line[pos] == '#' && (pos = NPOS))
			continue ;
		if (line[pos] != '"')
			return -1;
		if ((i = line.find("\"", pos + 1)) == NPOS)
			return -1;
		std::string	 realname(line, pos + 1, i - pos - 1);
		names.push_back(realname);
		pos = line.find_first_not_of(" \t\n", i + 1);
		if (pos == NPOS || line[pos] != ';')
			return -1;
		pos = line.find_first_not_of(" \t\n", pos + 1);
		if (pos != NPOS && line[pos] != '#' && line[pos] != '"' &&
			!(k == 1 && line[pos] == '}'))
			return -1;
		if (pos < line.length() && line[pos] == '#')	// hmmmmmmmmm
			pos = NPOS;
		k--;
	}
	while ((pos = line.find_first_not_of(" \t\n", pos)) == NPOS)
	{
		getline(config, line);
		line_number++;
		if (config.eof())
			return -1;
		pos = 0;
	}
	if (line[pos] != '}')
		return -1;
	pos = line.find_first_not_of(" \t\n", pos + 1);
	if (pos != NPOS && line[pos] != '#')
		return -1;
	serv->servername = names[0];
	serv->token = names[1];
	serv->info = names[2];
#if DEBUG_MODE
	std::cout << "server name: \"" << serv->servername << '"';
	std::cout << " server token: \"" << serv->token << '"';
	std::cout << " server info: \"" << serv->info << '"' << '\n';
#endif
	return 0;
}

int
block_oper(std::ifstream &config, std::string &line, IRCserv *serv, size_t &line_number)
{
	size_t	pos;
	size_t	i;
	t_oper	temp;

	pos = line.find_first_not_of(" \t");
	pos += ft_strlen("oper");
	while ((pos = line.find_first_not_of(" \t\n", pos)) == NPOS || line[pos] == '#')
	{
		getline(config, line);
		line_number++;
		if (config.eof())
			return -1;
		pos = 0;
	}
	if (line[pos] != '{' || config.eof())
		return -1;
	pos++;
	while (pos >= line.length() || line[pos] != '}')
	{
		while ((pos = line.find_first_not_of(" \t\n", pos)) == NPOS)
		{
			getline(config, line);
			line_number++;
			if (config.eof())
				return -1;
			pos = 0;
		}
		if (line[pos] == '#' && (pos = NPOS))
			continue ;
		if (!line.compare(pos, 4, "name"))
		{
			pos = line.find_first_not_of(" \n\t", pos + ft_strlen("name"));
			if (pos == NPOS || line[pos] != '"')
				return -1;
			if ((i = line.find("\"", pos + 1)) == NPOS)
				return -1;
			std::string name(line, pos + 1, i - pos - 1);
			temp.name = name;
			pos = line.find_first_not_of(" \t\n", i + 1);
			if (pos == NPOS || line[pos] != ';')
				return -1;
			pos = line.find_first_not_of(" \t\n", pos + 1);
			if (pos != NPOS && line[pos] != '#' && line[pos] != '}' && line.compare(pos, 4, "pass") && line.compare(pos, 6, "swhois"))
				return -1;
		}
		else if (!line.compare(pos, 4, "pass"))
		{
			pos = line.find_first_not_of(" \n\t", pos + ft_strlen("pass"));
			if (pos == NPOS || line[pos] != '"')
				return -1;
			if ((i = line.find("\"", pos + 1)) == NPOS)
				return -1;
			std::string pass(line, pos + 1, i - pos - 1);
			temp.pass = pass;
			pos = line.find_first_not_of(" \t\n", i + 1);
			if (pos == NPOS || line[pos] != ';')
				return -1;
			pos = line.find_first_not_of(" \t\n", pos + 1);
			if (pos != NPOS && line[pos] != '#' && line[pos] != '}' && line.compare(pos, 4, "name") && line.compare(pos, 6, "swhois"))
				return -1;
		}
		else if (!line.compare(pos, 6, "swhois"))
		{
			pos = line.find_first_not_of(" \n\t", pos + ft_strlen("swhois"));
			if (pos == NPOS || line[pos] != '"')
				return -1;
			if ((i = line.find("\"", pos + 1)) == NPOS)
				return -1;
			std::string pass(line, pos + 1, i - pos - 1);
			temp.swhois = pass;
			pos = line.find_first_not_of(" \t\n", i + 1);
			if (pos == NPOS || line[pos] != ';')
				return -1;
			pos = line.find_first_not_of(" \t\n", pos + 1);
			if (pos != NPOS && line[pos] != '#' && line[pos] != '}' && line.compare(pos, 4, "name") && line.compare(pos, 4, "pass"))
				return -1;
		}
		else if (!line.compare(pos, 8, "hostmask"))
		{
			if ((pos = get_arg(pos, line, temp.hostmask, "hostmask")) == NPOS)
                return -1;
            pos = line.find_first_not_of(" \t\n", pos + 1);
            if (pos != NPOS && line[pos] != '#' && line[pos] != '}' &&
                    line.compare(pos, 4, "pass") &&
                    line.compare(pos, 4, "name") &&
                    line.compare(pos, 4, "swhois"))
                return -1;
		}
		else if (!line.compare(pos, 1, "#"))
			pos = NPOS;
		else if (!line.compare(pos, 1, "}"))
			break ;
		else
			return -1;
	}

	//	default hostmask for all operators
	serv->oper.push_back(temp);
#if DEBUG_MODE
	std::cout << "operator: name '" << temp.name <<
				"' pass: '" << temp.pass <<
				"' whois: '" << temp.swhois <<
				"' hostmask: '" << temp.hostmask << "'" << std::endl;
#endif
	return 0;
}

int
block_service(std::ifstream &config, std::string &line, IRCserv *serv, size_t &line_number)
{
	size_t	pos;
	t_service	temp;

	pos = line.find_first_not_of(" \t");
	pos += ft_strlen("service");
	while ((pos = line.find_first_not_of(" \t\n", pos)) == NPOS || line[pos] == '#')
	{
		getline(config, line);
		line_number++;
		if (config.eof())
			return -1;
		pos = 0;
	}
	if (line[pos] != '{' || config.eof())
		return -1;
	pos++;
	while (pos >= line.length() || line[pos] != '}')
	{
		while ((pos = line.find_first_not_of(" \t\n", pos)) == NPOS)
		{
			getline(config, line);
			line_number++;
			if (config.eof())
				return -1;
			pos = 0;
		}
		if (line[pos] == '#' && (pos = NPOS))
			continue ;
		if (!line.compare(pos, 4, "name"))
		{
			if ((pos = get_arg(pos, line,  temp.name, "name")) == NPOS)
				return -1;
			pos = line.find_first_not_of(" \t\n", pos + 1);
			if (pos != NPOS && line[pos] != '#' && line[pos] != '}' &&
					line.compare(pos, 4, "pass") &&
					line.compare(pos, 4, "type") &&
					line.compare(pos, 12, "distribution") &&
					line.compare(pos, 4, "info"))
				return -1;
		}
		else if (!line.compare(pos, 4, "pass"))
		{
			if ((pos = get_qarg(pos, line, temp.pass, "pass")) == NPOS)
				return -1;
			pos = line.find_first_not_of(" \t\n", pos + 1);
			if (pos != NPOS && line[pos] != '#' && line[pos] != '}' &&
					line.compare(pos, 4, "distribution") &&
                    line.compare(pos, 4, "type") &&
                    line.compare(pos, 4, "name") &&
                    line.compare(pos, 4, "info") &&
                    line.compare(pos, 8, "hostmask"))
				return -1;
		}
		else if (!line.compare(pos, 12, "distribution"))
		{
			if ((pos = get_arg(pos, line, temp.distribution, "distribution")) == NPOS)
				return -1;
			pos = line.find_first_not_of(" \t\n", pos + 1);
			if (pos != NPOS && line[pos] != '#' && line[pos] != '}' &&
					line.compare(pos, 4, "pass") &&
                    line.compare(pos, 4, "type") &&
                    line.compare(pos, 4, "name") &&
                    line.compare(pos, 4, "info") &&
					line.compare(pos, 8, "hostmask"))
				return -1;
		}
		else if (!line.compare(pos, 4, "type"))
		{
			if ((pos = get_arg(pos, line, temp.type, "type")) == NPOS)
				return -1;
			pos = line.find_first_not_of(" \t\n", pos + 1);
			if (pos != NPOS && line[pos] != '#' && line[pos] != '}' &&
					line.compare(pos, 4, "pass") &&
                    line.compare(pos, 4, "name") &&
                    line.compare(pos, 12, "distribution") &&
                    line.compare(pos, 4, "info") &&
					line.compare(pos, 8, "hostmask"))
				return -1;
		}
		else if (!line.compare(pos, 4, "info"))
		{
			if ((pos = get_qarg(pos, line, temp.info, "type")) == NPOS)
				return -1;
			pos = line.find_first_not_of(" \t\n", pos + 1);
			if (pos != NPOS && line[pos] != '#' && line[pos] != '}' &&
					line.compare(pos, 4, "pass") &&
                    line.compare(pos, 4, "name") &&
                    line.compare(pos, 12, "distribution") &&
                    line.compare(pos, 4, "info") &&
					line.compare(pos, 8, "hostmask"))
				return -1;
		}
		else if (!line.compare(pos, 8, "hostmask"))
		{
			if ((pos = get_arg(pos, line, temp.hostmask, "hostmask")) == NPOS)
				return -1;
			pos = line.find_first_not_of(" \t\n", pos + 1);
			if (pos != NPOS && line[pos] != '#' && line[pos] != '}' &&
					line.compare(pos, 4, "pass") &&
					line.compare(pos, 4, "type") &&
                    line.compare(pos, 4, "name") &&
                    line.compare(pos, 12, "distribution") &&
                    line.compare(pos, 4, "info"))
				return -1;
		}

		else if (!line.compare(pos, 1, "#"))
			pos = NPOS;
		else if (!line.compare(pos, 1, "}"))
			break ;
		else
			return -1;
	}
	temp.fd = -1;
	serv->services.push_back(temp);
#if DEBUG_MODE
	std::cout << "service: name '" << temp.name <<
				"' pass: '" << temp.pass <<
				"' distribution: '" << temp.distribution <<
				"' hostmask: '" << temp.hostmask <<
				"' info: '" << temp.info <<
				"' type: '" << temp.type << "'" << std::endl;
#endif
	return 0;
}

int
block_motd(std::ifstream &config, std::string &line, IRCserv *serv, size_t &line_number)
{
	size_t		pos;
	size_t		i;
	t_strvect	names;

	pos = line.find_first_not_of(" \t");
	pos += ft_strlen("motd");
	while ((pos = line.find("{", pos)) == NPOS)
	{
		getline(config, line);
		line_number++;
		if (config.eof())
			return -1;
		pos = 0;
	}
	if (line[pos] != '{' || config.eof())
		return -1;
	pos++;
	if (pos >= line.length() || line[pos] != '"')	// hmmmmmmm
		while ((pos = line.find_first_not_of(" \t\n", pos)) == NPOS)
		{
			getline(config, line);
			line_number++;
			if (config.eof())
				return -1;
			pos = 0;
		}
	if (line[pos] != '"')
		return -1;
	if ((i = line.find("\"", pos + 1)) == NPOS)
		return -1;
	std::string	real_deal(line, pos + 1, i - pos - 1);
	serv->motd_path = real_deal;
	pos = line.find_first_not_of(" \t\n", i + 1);
	if (pos == NPOS || line[pos] != ';')
		return -1;
	pos = line.find_first_not_of(" \t\n", pos + 1);
	if (pos != NPOS && line[pos] != '#' && line[pos] != '"' &&
			!(line[pos] == '}'))
		return -1;
	while ((pos = line.find_first_not_of(" \t\n", pos)) == NPOS)
	{
		getline(config, line);
		line_number++;
		if (config.eof())
			return -1;
		pos = 0;
	}
	if (line[pos] != '}')
		return -1;
	pos = line.find_first_not_of(" \t\n", pos + 1);
	if (pos != NPOS && line[pos] != '#')
		return -1;
#if DEBUG_MODE
	std::cout << "MOTD: '" << serv->motd_path << '\'' << '\n';
#endif
	return 0;

}

void	server_init(IRCserv *serv, int ac, char **av)
{
	bool	flag_for_serv_connection;
	t_link	link;

	serv->port = -1;
	flag_for_serv_connection = false;
	if (ac == 2)
	{

		std::string port(av[1]);

		if (port.find_first_not_of("0123456789") != NPOS ||
				port.length() < 1 || port.length() > 5)
			error_exit("Error: bad port number");
		serv->port = STOI(port);
		if (serv->port < 1 || serv->port > 65535)
			error_exit("Error: bad port number");
	}
	else if (ac == 3)
	{
		std::string port(av[1]);
		std::string pass(av[2]);

		if (port.find_first_not_of("0123456789") == NPOS)
		{
			if (port.length() < 1 || port.length() > 5)
				error_exit("Error: bad port number");
			else
			{
				serv->pass = pass;
				serv->port = STOI(port);
				if (serv->port < 1 || serv->port > 65535)
					error_exit("Error: bad port number");
			}
		}
		else
		{
			t_strvect	temp;

			flag_for_serv_connection = true;
			temp = ft_splitstring(av[1], ":");
			if (temp.size() < 2 || temp.size() > 3)
				error_exit("Error: input parameters of server which you want to connect to is wrong!");
			link.hostname = temp[0];
			if (temp[1].find_first_not_of("0123456789") != NPOS ||
										temp.size() < 1 || temp.size() > 5)
								error_exit("Error: bad port number");
			link.port = STOI(temp[1]);
			if (temp.size() == 3)
				link.pass = temp[2];
			if (pass.find_first_not_of("0123456789") != NPOS ||
					pass.length() < 1 || pass.length() > 5)
				error_exit("Error: bad port number");
			else
			{
				serv->port = STOI(pass);
				if (serv->port < 1 || serv->port > 65535)
					error_exit("Error: bad port number");
			}
		}
	}
	else if (ac == 4)
	{
		std::string port(av[2]);
		std::string pass(av[3]);
		t_strvect	connect_to;

		flag_for_serv_connection = true;
		connect_to = ft_splitstring(av[1], ":");
		if (connect_to.size() < 2 || connect_to.size() > 3)
			error_exit("Error: input parameters of server which you want to connect to is wrong!");
		link.hostname = connect_to[0];
		serv->pass = pass;
		if (port.find_first_not_of("0123456789") != NPOS ||
		connect_to[1].find_first_not_of("0123456789") != NPOS ||
		connect_to[1].length() < 1 || connect_to[1].length() > 5 ||
		port.length() < 1 || port.length() > 5)
			error_exit("Error: bad port number");
		else
		{
			link.port = STOI(connect_to[1]);
			if (connect_to.size() == 3)
				link.pass = connect_to[2];
			serv->port = STOI(port);
			if (serv->port < 1 || serv->port > 65535)
				error_exit("Error: bad port number");
		}
	}
	else
		usage_exit(av[0]);
	if (flag_for_serv_connection)
	{
		std::vector<t_link>::iterator	b = serv->link.begin();
		std::vector<t_link>::iterator	e = serv->link.end();
		int	flag;

		flag = 0;
		while (b != e)
		{
			if (b->hostname == link.hostname &&
					b->port == link.port &&
					b->pass == link.pass)
			{
				b->autoconnect = true;
				flag = 1;
				break ;
			}
			b++;
		}
		if (!flag)
			error_exit("Error: server you are trying to connect is bad configured");
	}
}

size_t	find_block(std::string line, size_t pos)
{
	if (!line.compare(pos, ft_strlen("listen"), "listen"))
		return LISTEN;
	if (!line.compare(pos, ft_strlen("admin"), "admin"))
		return ADMIN;
	if (!line.compare(pos, ft_strlen("link"), "link"))
		return LINK;
	if (!line.compare(pos, ft_strlen("me"), "me"))
		return ME;
	if (!line.compare(pos, ft_strlen("oper"), "oper"))
		return OPER;
	if (!line.compare(pos, ft_strlen("motd"), "motd"))
		return MOTD;
	if (!line.compare(pos, ft_strlen("service"), "service"))
		return SERVICE;
	return NPOS;
}

void	parse(int ac, char **av, IRCserv *serv)
{
	std::ifstream	config;
	std::string		line;
	size_t			line_number;
	size_t			i;
	t_blockmap		block;

	serv->admin.set = false;
	block[LISTEN] = block_listen;
	block[ADMIN] = block_admin;
	block[LINK] = block_link;
	block[OPER] = block_oper;
	block[ME] = block_me;
	block[MOTD] = block_motd;
	block[SERVICE] = block_service;
	config.open("./conf/ircserv.conf", std::ios::in);
	if (!config.is_open())
		error_exit("Error: can't open file \"./config/ircserv.conf\"");
	line_number = 0;
	while (!config.eof())
	{
		line_number++;
		getline(config, line);
		i = line.find_first_not_of(" \t\n");
		if (i == NPOS || line[i] == '#')
			continue ;
		i = find_block(line, i);
		if (i == NPOS)
			error_exit("Error: config error at line ", line, line_number);
		else
			if ((block[i](config, line, serv, line_number)) == -1)
				error_exit("Error: config error at line ", line, line_number);
	}
	config.close();
	server_init(serv, ac, av);
}
