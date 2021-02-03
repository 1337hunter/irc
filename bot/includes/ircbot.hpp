/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircbot.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/09 18:57:38 by salec             #+#    #+#             */
/*   Updated: 2021/02/04 00:14:57 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCBOT_HPP
#define IRCBOT_HPP

#include <iostream>
#include <string>
#include <map>

#include "common.hpp"
#include "bottools.hpp"
#include "cmds.hpp"

class Command;

typedef std::map<std::string, Command>	t_cmdmap;

struct	ircbot {
	int const	sock;
	char		prefix;
	std::string	botname;
	std::string	version;
	t_cmdmap	cmds;
	bool		asService;

	ircbot(int sock, bool asService);
	~ircbot();
};

bool	checkmsg(t_strvect const &split, ircbot const &bot);
void	ReceiveMessages(ircbot const &bot);

#endif
