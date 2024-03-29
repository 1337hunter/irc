#include "ircserv.hpp"
#include "message.hpp"
#include "tools.hpp"

void	cmd_invite(int fd, t_strvect const &split, IRCserv *serv)
{
	std::list<Channel>::iterator	chan;
	Client							*invite_from = 0;
	Client							*invite_whom = 0;
	std::string						chan_name;
	std::list<Channel*>::iterator	client_chan;

	if (split.size() < 3)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, fd, "INVITE",
				"Not enough parameters"); return ;
	}
	if (split.size() > 3 && split[0][0] == ':' && serv->fds[fd].type == FD_SERVER)
	{
		if (!(invite_from = find_client_by_nick(get_nick_from_info(split[0]), serv)))
			return ;
		if (!(invite_whom = find_client_by_nick(split[2], serv)))
			return ;
		if (invite_whom->gethop() == 0)
			serv->fds[invite_whom->getFD()].wrbuf += ":" + invite_from->getinfo() +
			" INVITE " + invite_whom->getnick() + " " + split[3] + CRLF;
		else
			msg_forward(fd, strvect_to_string(split), serv);
		invite_whom->invite_to(split[3]);
		return ;
	}
	else if (split.size() == 3 && (serv->fds[fd].type == FD_CLIENT ||
				serv->fds[fd].type == FD_OPER))
	{
		invite_from = find_client_by_fd(fd, serv);
		invite_whom = find_client_by_nick(split[1], serv);
		chan_name = split[2];
	}
	else
		return ;
	if (invite_from == 0 || !invite_from->isRegistred())
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1, "",
				"You have not registered"); return ;
	}
	for (chan = serv->channels.begin(); chan != serv->channels.end(); chan++)
		if (chan->getname() == split[2])
			break ;
	if (invite_whom == 0 || !invite_whom->isRegistred() || chan == serv->channels.end())
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHNICK, fd, "INVITE",
				"No such nick/channel");
		return ;
	}
	client_chan = (invite_whom->getchannels()).begin();
	for ( ; client_chan != invite_whom->getchannels().end(); client_chan++)
		if ((*client_chan)->getname() == chan_name)
			break ;
	if (client_chan != invite_whom->getchannels().end())
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_USERONCHANNEL, fd,
		invite_whom->getnick() + " " + chan_name, "is already on channel"); return ;
	}
	client_chan = invite_from->getchannels().begin();
	for ( ; client_chan != invite_from->getchannels().end(); client_chan++)
		if ((*client_chan)->getname() == chan_name)
			break ;
	if (client_chan == invite_from->getchannels().end())
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTONCHANNEL, fd, chan_name,
				"You're not on that channel"); return ;
	}
	if ((*client_chan)->getflags()._invite_only &&
		!((*client_chan)->getclients())[invite_from]._operator &&
		!((*client_chan)->getclients())[invite_from]._Operator)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_CHANOPRIVSNEEDED, fd, chan_name,
				"You're not channel operator"); return ;
	}
	invite_whom->invite_to(chan_name);
	if (invite_whom->gethop() == 0)
		serv->fds[invite_whom->getFD()].wrbuf += ":" + invite_from->getinfo() +
		" INVITE " + split[1] + " " + chan_name + CRLF;
	else
		serv->fds[invite_whom->getFD()].wrbuf += ":" + invite_from->getnick() +
		" INVITE " + split[1] + " " + chan_name + CRLF;
	serv->fds[fd].wrbuf += ":" + serv->servername + " 341 " + invite_from->getnick() +
		" " + invite_whom->getnick() + " " + chan_name + CRLF;
}
