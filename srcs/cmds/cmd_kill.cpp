#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"

void	kill_from_operator(int fd, t_strvect const &split, IRCserv *serv)
{
	Client	*client;
	Client	*client_from;
	std::list<Channel*>::iterator	chan;
	std::string						cause;
	t_kill							kill;

	if (!(client = find_client_by_nick(split[1], serv)))
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHNICK, fd, split[1],
			"No such nick/channel"); return ;
	}
	if (!(client_from = find_client_by_fd(fd, serv)))
		return ;
	cause = strvect_to_string(split, ' ', 2);
	chan = client->getchannels().begin();
	for (; chan != client->getchannels().end(); chan++)
		msg_to_channel(*chan, client, "PART " + (*chan)->getname() + " " + cause, serv);
	chan = client->getchannels().begin();
	for (; chan != client->getchannels().end(); chan++)
		(*chan)->getclients().erase(client);
	serv->fds[client->getFD()].wrbuf += ":" + client->getinfo() + " QUIT " + cause + CRLF;
	msg_forward(fd, strvect_to_string(split), serv);
	msg_forward(fd, ":" + client->getnick() + " QUIT " + cause, serv);
	serv->fds[client->getFD()].status = false;
	remove_client_by_ptr(client, serv);
	kill.nick = client->getnick();
	kill.host = client->gethostname();
	kill.time = ft_getcurrenttime();
	kill.cause = cause;
	serv->kills.push_back(kill);
}

void	kill_from_network(int fd, t_strvect const &split, IRCserv *serv)
{
	t_kill	kill;
	Client	*client;

	if (split.size() < 3)
		return ;
	if (!(client = find_client_by_nick(split[1], serv)))
		return ;
	kill.nick = client->getnick();
	kill.host = client->gethostname();
	kill.time = ft_getcurrenttime();
	kill.cause = strvect_to_string(split, ' ', 2);
	msg_forward(fd, strvect_to_string(split), serv);
}

void	cmd_kill(int fd, t_strvect const &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_CLIENT)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOPRIVILEGES, fd, "",
			"Permission Denied- You're not an IRC operator"); return ;
	}
	if (split.size() < 3)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, fd, "KILL",
			"Not enough parameters"); return ;
	}
	if (serv->fds[fd].type == FD_SERVER)
		kill_from_network(fd, split, serv);
	else
		kill_from_operator(fd, split, serv);
}
