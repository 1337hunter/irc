/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_version.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/13 19:07:03 by salec             #+#    #+#             */
/*   Updated: 2021/01/13 21:28:15 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cmds.hpp"

std::string	cmd_version(t_strvect const &split, ircbot const &bot)
{
	(void)split;
	std::string	res = bot.botname + " v" + bot.version;
	return (res);
}
