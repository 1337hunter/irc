#include "cmds.hpp"

std::string	cmd_version(t_strvect const &split, ircbot const &bot)
{
	(void)split;
	std::string	res = bot.botname + " v" + bot.version;
	return (res);
}
