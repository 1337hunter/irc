/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbright <gbright@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/27 15:41:07 by gbright           #+#    #+#             */
/*   Updated: 2020/10/27 16:34:12 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

void	cmd_nick(int fd, const t_strvect &split, IRCserv *_server);
void    cmd_user(int fd, const t_strvect &split, IRCserv *_server);
void    cmd_ping(int fd, const t_strvect &split, IRCserv *_server);
