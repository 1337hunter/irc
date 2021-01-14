/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircbot.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/09 18:57:38 by salec             #+#    #+#             */
/*   Updated: 2021/01/14 19:01:20 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCBOT_HPP
#define IRCBOT_HPP

#include <iostream>
#include <string>
#include <map>

#include "common.hpp"
#include "tools.hpp"
#include "cmds.hpp"

class Command;

typedef std::map<std::string, Command>	t_cmdmap;

struct	ircbot {
	int const	sock;
	char		prefix;
	std::string	botname;
	std::string	version;
	t_cmdmap	cmds;

	ircbot(int sock);
	~ircbot();
};

bool	checkmsg(t_strvect const &split, ircbot const &bot);
void	ReceiveMessages(ircbot const &bot);

#endif
