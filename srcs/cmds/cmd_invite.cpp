#include "ircserv.hpp"
#include "message.hpp"
#include "tools.hpp"
//Command: INVITE
//Parameters: <nickname> <channel>

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
				"Not enough parameters"); return ;
	}
	if (split.size() == 4 && split[0][0] == ':' && serv->fds[fd].type == FD_SERVER)
	{
		invite_from = find_client_by_nick(std::string(split[0], 1), serv);
		invite_who = find_client_by_nick(split[2], serv);
		// They are defenitly registred !
		serv->fds[invite_who->getFD()].wrbuf += ":" + invite_from->getnickname() +
			"!" + invite_from->getusername() + "@" + invite_from->gethostname() +
			" INVITE " + invite_who->getnickname() + " " + split[3] + CRLF;
		// I dont care about routing
		return ;
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
				"You have not registered"); return ;
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
		invite_who->getnickname() + " " + chan_name, "is already on channel"); return ;
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
	if ((*client_chan)->getchanflags()._invite_only &&
		!((*client_chan)->getclients())[invite_from]._operator &&
		!((*client_chan)->getclients())[invite_from]._Operator)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_CHANOPRIVSNEEDED, fd, chan_name,
				"You're not channel operator"); return ;
	}
	invite_who->invite_to(chan_name);
	serv->fds[invite_who->getFD()].wrbuf += ":" + invite_from->getnickname() +
		"!" + invite_from->getusername() + "@" + invite_from->gethostname() +
		" INVITE " + split[1] + " " + chan_name + CRLF; 
	//if ^^^^ hop > 1 and getFD = serv_routin || client_fd	I dont care about either
	//client on THIS server or on the ENOTHER server
	serv->fds[fd].wrbuf += ":" + serv->servername + " 341 " + invite_from->getnickname() +
		" " + invite_who->getnickname() + " " + chan_name + CRLF;
}
