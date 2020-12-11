/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbright <gbright@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/10 21:05:39 by gbright           #+#    #+#             */
/*   Updated: 2020/12/11 14:38:20 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include "ircserv.hpp"

std::string	get_reply(IRCserv *_server, std::string _errno, int fd, std::string const &command, const std::string &message);
std::string	get_reply(IRCserv *serv, std::string rpl, Client *client, std::string command, std::string message);
	
void	msg_error(std::string msg, IRCserv *_server);
void	msg_error(std::string msg, std::string msg1, IRCserv *_server);
void	msg_forward(int fd, std::string const &msg, IRCserv *serv);
void    msg_to_channel(Channel *channel, std::string msg, IRCserv *serv);
#endif
