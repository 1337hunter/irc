#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"

void	kill_from_operator(int fd, t_strvect const &split, IRCserv *serv)
{
	Client	*client;
	Client	*client_from;
	std::string						cause;
	t_kill							kill;

	if (!(client = find_client_by_nick(split[1], serv)))
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHNICK, fd, split[1],
			"No such nick/channel"); return ;
	}
	if (!(client_from = find_client_by_fd(fd, serv)))
		return ;
	cause = strvect_to_string(split, ' ', 2) + " (KILL from " + client_from->getnick() + ")";
	cause = cause.substr(1);
	msg_forward(fd, strvect_to_string(split), serv);
	if (client->gethop() == 0)
		self_cmd_quit(client->getFD(), serv->fds[client->getFD()], serv, cause);
	else
		msg_forward(fd, ":" + client->getnick() + " QUIT " + cause, serv);
	kill.nick = client->getnick();
	kill.host = client->gethostname();
	kill.time = ft_getcurrenttime();
	kill.cause = cause;
	serv->kills.push_back(kill);
	if (client->gethop() != 0)
	{
		client->partAllChan();
		remove_client_by_ptr(client, serv);
	}
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
	serv->kills.push_back(kill);
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
