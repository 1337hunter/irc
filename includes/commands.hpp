/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/27 15:41:07 by gbright           #+#    #+#             */
/*   Updated: 2020/12/07 18:27:32 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_HPP
# define COMMANDS_HPP

# include <string>

void	cmd_nick(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_user(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_ping(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_quit(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_oper(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_server(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_pass(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_squit(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_connect(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_error(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_admin(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_motd(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_userhost(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_version(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_info(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_time(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_join(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_privmsg(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_invite(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_names(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_who(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_whois(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_whowas(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_part(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_mode(int fd, const t_strvect &split, IRCserv *serv);

std::string		reply_welcome(IRCserv *serv, t_citer const cli);
std::string		reply_motd(IRCserv *serv, std::string const &it);
std::string		reply_chan_names(IRCserv *serv, std::list<Channel>::iterator chan, Client *client);
std::string		reply_nochan_visible_names(IRCserv *serv, Client *client);
bool			is_server_registred(const std::string &name, IRCserv *serv);
std::string		getmatchingservername(IRCserv *serv, std::string const &mask);

typedef std::vector<t_server>::iterator	t_netit;

#endif
