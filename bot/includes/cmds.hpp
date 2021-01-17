/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmds.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/13 16:39:52 by salec             #+#    #+#             */
/*   Updated: 2021/01/14 17:34:35 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CMDS_HPP
#define CMDS_HPP

#include "ircbot.hpp"
#include "tools.hpp"
#include "common.hpp"

struct	ircbot;

class	Command {
private:
	typedef std::string (*t_cmd)(t_strvect const &split, ircbot const &bot);

	t_cmd		cmd;
	std::string	info;
	Command(t_cmd cmd);

public:
	Command();
	Command(t_cmd cmd, std::string const &info);
	Command(Command const &other);
	~Command();
	Command	&operator=(Command const &other);

	std::string const	&getinfo(void) const;

	void		setinfo(std::string const &info);
	std::string	Execute(t_strvect const &split, ircbot const &bot) const;
};

std::string		cmd_info(t_strvect const &split, ircbot const &bot);
std::string		cmd_help(t_strvect const &split, ircbot const &bot);
std::string		cmd_version(t_strvect const &split, ircbot const &bot);
std::string		cmd_weather(t_strvect const &split, ircbot const &bot);

#endif
