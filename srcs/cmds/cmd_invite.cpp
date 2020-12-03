#include "ircserv.hpp"
#include "message.hpp"
#include "tools.hpp"
//Command: INVITE
//Parameters: <nickname> <channel>
//
void	cmd_invite(int fd, t_strvect const &split, IRCserv *serv)
{
	std::list<Channel>::iterator	chan;
	Client							*invite_from = 0;
	Client							*invite_who = 0;
	std::string						chan_name;
	std::list<Channel*>::iterator	client_chan;

	if (split.size() < 3)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, fd, "INVITE",
				"Not enough parameters");
		return ;
	}
	if (split.size() == 4 && split[0][0] == ':' && serv->fds[fd].type == FD_SERVER)
	{
		invite_from = find_client_by_nick(std::string(split[0], 1), serv);
		invite_who = find_client_by_nick(split[2], serv);
		chan_name = split[3];
	}
	else if (split.size() == 3 && serv->fds[fd].type == FD_CLIENT)//type is just in case
	{
		invite_from = find_client_by_fd(fd, serv);
		invite_who = find_client_by_nick(split[1], serv);
		chan_name = split[2];
	}
	else
		return ;
	if (invite_from == 0 || !invite_from->isRegistred())
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1, "",
				"You have not registered");
		return ;
	}
	for (chan = serv->channels.begin(); chan != serv->channels.end(); chan++)
		if (chan->getname() == split[2])
			break ;
	if (invite_who == 0 || !invite_who->isRegistred() || chan == serv->channels.end())
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHNICK, fd, "INVITE",
				"No such nick/channel"); 
		//if fd.type == FD_SERVER ned to resend error to nickname
		return ;
	}
	client_chan = (invite_who->getchannels()).begin();
	for ( ; client_chan != invite_who->getchannels().end(); client_chan++)
		if ((*client_chan)->getname() == chan_name)
			break ;
	if (client_chan != invite_who->getchannels().end())
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_USERONCHANNEL, fd,
				invite_who->getnickname() + " " + chan_name, "is already on channel");
		return ;
	}
	client_chan = invite_from->getchannels().begin();
	for ( ; client_chan != invite_from->getchannels().end(); client_chan++)
		if ((*client_chan)->getname() == chan_name)
			break ;
	if (client_chan == invite_from->getchannels().end())
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_USERONCHANNEL, fd, chan_name,
				"You're not on that channel");
		return ;
	}
}
