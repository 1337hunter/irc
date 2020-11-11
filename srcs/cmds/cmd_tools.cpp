/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_tools.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbright <gbright@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/11 17:08:35 by gbright           #+#    #+#             */
/*   Updated: 2020/11/11 17:12:21 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

std::string     reply_welcome(IRCserv *serv, t_citer it)
{
    std::string reply;

    reply += ft_buildmsg(serv->hostname, RPL_WELCOME,
            it->getnickname(), "",
            "Welcome to the Internet Relay Network " +
            it->getnickname() + "!" + it->getnickname() + "@" +
            serv->hostname);
    reply += ft_buildmsg(serv->hostname, RPL_YOURHOST,
            it->getnickname(), "", "Your host is " + serv->hostname +
            ", running version " + /*serv->version*/ "0.1");
    reply += ft_buildmsg(serv->hostname, RPL_CREATED,
                it->getnickname(), "", "This server was created <date>");
    reply += ft_buildmsg(serv->hostname, RPL_MYINFO,
                it->getnickname(), "", serv->hostname + " " +
                /*serv->version*/ "0.1" + " " +
                "<available user modes>" + " " +
                "<available channel modes>");
    return reply;
}
