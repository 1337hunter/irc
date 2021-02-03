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
