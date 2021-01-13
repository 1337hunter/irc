/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmds.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/13 20:36:36 by salec             #+#    #+#             */
/*   Updated: 2021/01/13 21:37:42 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cmds.hpp"

Command::Command() : cmd(NULL), description("") {}

Command::Command(t_cmd cmd) : cmd(cmd), description("") {}

Command::Command(t_cmd cmd, std::string const &description) :
	cmd(cmd), description(description) {}

Command::Command(Command const &other) :
	cmd(other.cmd), description(other.description) {}

Command::~Command() {}

Command	&Command::operator=(Command const &other)
{
	this->cmd = other.cmd;
	this->description = other.description;
	return (*this);
}

std::string const	&Command::getdescription(void) const
{
	return (this->description);
}

void		Command::setdescription(std::string const &description)
{
	this->description = description;
}

std::string	Command::Execute(t_strvect const &split, ircbot const &bot) const
{
	if (this->cmd)
		return (cmd(split, bot));
	return ("");
}
