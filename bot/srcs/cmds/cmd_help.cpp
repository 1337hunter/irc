#include "cmds.hpp"

std::string	cmd_help(t_strvect const &split, ircbot const &bot)
{
	(void)split;
	std::string	res = "";

	for (t_cmdmap::const_iterator it = bot.cmds.begin();
		it != bot.cmds.end(); it++)
		res += bot.prefix + it->first + " - " + it->second.getinfo() + '\n';

	return (res);
}
