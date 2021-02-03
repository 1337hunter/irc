#include "cmds.hpp"

Command::Command() : cmd(NULL), info("") {}

Command::Command(t_cmd cmd) : cmd(cmd), info("") {}

Command::Command(t_cmd cmd, std::string const &info) :
	cmd(cmd), info(info) {}

Command::Command(Command const &other) :
	cmd(other.cmd), info(other.info) {}

Command::~Command() {}

Command	&Command::operator=(Command const &other)
{
	this->cmd = other.cmd;
	this->info = other.info;
	return (*this);
}

std::string const	&Command::getinfo(void) const
{
	return (this->info);
}

void		Command::setinfo(std::string const &info)
{
	this->info = info;
}

std::string	Command::Execute(t_strvect const &split, ircbot const &bot) const
{
	if (this->cmd)
		return (cmd(split, bot));
	return ("");
}
