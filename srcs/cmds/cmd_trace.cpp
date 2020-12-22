#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"

void	trace_from_network(int fd, t_strvect const &split, IRCserv *serv)
{
	fd = 0; (void)split; serv = 0;
}

void	trace_from_client(int fd, t_strvect const &split, IRCserv *serv)
{
	Client	*client;

	if (!(client = find_client_by_fd(fd, serv)) || !client->isRegistred())
	{
		serv->fds[fd].wrbuf += get_reply(serv, "451", -1, "", "You have not registered");
		return ;
	}
	if (split.size() == 1)
	{

	}
}

void	cmd_trace(int fd, t_strvect const &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_SERVER)
		trace_from_network(fd, split, serv);
	else
		trace_from_client(fd, split, serv);
}
