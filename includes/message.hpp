/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbright <gbright@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/10 21:05:39 by gbright           #+#    #+#             */
/*   Updated: 2020/11/12 22:30:24 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include "ircserv.hpp"

std::string	get_reply(IRCserv *_server, std::string _errno, int fd, std::string const &command, const std::string &message);

void	msg_error(std::string msg, IRCserv *_server);
void	msg_error(std::string msg, std::string msg1, IRCserv *_server);
#endif
