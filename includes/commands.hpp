/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/27 15:41:07 by gbright           #+#    #+#             */
/*   Updated: 2020/11/13 13:14:59 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_HPP
# define COMMANDS_HPP

void			cmd_nick(int fd, const t_strvect &split, IRCserv *serv);
void			cmd_user(int fd, const t_strvect &split, IRCserv *serv);
void			cmd_ping(int fd, const t_strvect &split, IRCserv *serv);
void			cmd_quit(int fd, const t_strvect &split, IRCserv *serv);
void			cmd_oper(int fd, const t_strvect &split, IRCserv *serv);
void			cmd_server(int fd, const t_strvect &split, IRCserv *serv);
void			cmd_pass(int fd, const t_strvect &split, IRCserv *serv);
void			cmd_squit(int fd, const t_strvect &split, IRCserv *serv);
void			cmd_connect(int fd, const t_strvect &split, IRCserv *serv);
void			cmd_error(int fd, const t_strvect &split, IRCserv *serv);

std::string		reply_welcome(IRCserv *serv, t_citer const &it);
std::string		reply_motd(IRCserv *serv, t_citer const &it);

#endif
