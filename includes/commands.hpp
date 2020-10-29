/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/27 15:41:07 by gbright           #+#    #+#             */
/*   Updated: 2020/10/29 22:18:34 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_HPP
#define COMMANDS_HPP

void	cmd_nick(int fd, const t_strvect &split, IRCserv *_server);
void	cmd_user(int fd, const t_strvect &split, IRCserv *_server);
void	cmd_ping(int fd, const t_strvect &split, IRCserv *_server);
void	cmd_quit(int fd, const t_strvect &split, IRCserv *_server);
void	cmd_server(int fd, const t_strvect &split, IRCserv *_server);

#endif
