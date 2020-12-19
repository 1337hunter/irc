#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"

void	away_from_client(int fd, t_strvect const &split, IRCserv *serv)
{
	Client	*client;

	if (!(client = find_client_by_fd(fd, serv)) || !client->isRegistred())
	{
		serv->fds[fd].wrbuf += get_reply(serv, "451", -1, "", "You have not registered");
		return ;
	}
	if (split.size() == 1)
	{
		client->setUMODE("-a");
		serv->fds[fd].wrbuf += get_reply(serv, RPL_UNAWAY, client, "",
				"You are no longer marked as being away");
		msg_forward(fd, ":" + client->getinfo() + " MODE " + client->getnick() +
				" -a\r\n", serv);
	}
	else if (split.size() > 1)
	{
		client->setAway(strvect_to_string(split, 1));
		serv->fds[fd].wrbuf += get_reply(serv, RPL_NOWAWAY, client, "",
				"You have been marked as being away");
		msg_forward(fd, ":" + client->getinfo() + " MODE " + client->getnick() +
				" +a\r\n", serv);
	}
}

void	cmd_away(int fd, t_strvect const &split, IRCserv *serv)
{
	if (serv->fds[fd].type != FD_SERVER)
		away_from_client(fd, split, serv);
}
