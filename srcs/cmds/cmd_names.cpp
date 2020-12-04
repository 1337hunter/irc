/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_names.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbright <gbright@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/04 13:17:53 by gbright           #+#    #+#             */
/*   Updated: 2020/12/04 13:19:59 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "ircserv.hpp"
#include "tools.hpp"
#include "commands.hpp"

//Command: NAMES
//Parameters: [ <channel> *( "," <channel> ) [ <target> ] ]

void	cmd_names(int fd, const t_strvect &split, IRCserv *serv)
{
	t_strvect	chan_args;
	std::list<Channel>::iterator    chan;
	Client  *client;

	client = find_client_by_fd(fd, serv);
	if (client == 0)
		return ; // It must not be 0 but ...
	if (split.size() == 1)
		for (chan = serv->channels.begin(); chan != serv->channels.end(); chan++)
			serv->fds[fd].wrbuf += reply_chan_names(serv, chan, client);
	else if (split.size() == 2 && split[1] == "#")
	fd = 0;
	(void)split;
	serv = 0;
}
