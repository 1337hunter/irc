
#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"

void	squery_from_network(int fd, const t_strvect &split, IRCserv *serv)
{
	t_service	*service;

	if (split.size() < 4)
		return ;
	if (!(service = find_service_by_name(split[2], serv)) || service->fd < 0)
		return ;
	serv->fds[service->fd].wrbuf += split[0] + " " + split[1] + " " + split[2] + " :" +
		strvect_to_string(split, ' ', 3) + CRLF;
	fd = 0;
}

void	squery_from_client(int fd, const t_strvect &split, IRCserv *serv)
{
	Client		*client;
	t_service	*service;

	if (!(client = find_client_by_fd(fd, serv)))
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1, "",
			"You have not registered"); return ;
	}
	if (split.size() < 3)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, client, "SQUERY",
			"Not enough parameters"); return ;
	}
	if (!(service = find_service_by_name(split[1], serv)) || service->fd < 0)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHNICK, client, split[1],
			"No such service or service is offline"); return ;
	}
	serv->fds[service->fd].wrbuf += ":" + client->getnick() + " " + split[0] + " " +
		split[1] + " :" + strvect_to_string(split, ' ', 2) + CRLF;
#if DEBUG_MODE
	std::cout << "client " << fd <<
		" sent service query to " << split[1] <<
		" service:" << std::endl << split[2] << std::endl;
#endif

}

void	cmd_squery(int fd, const t_strvect &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_SERVER)
		squery_from_network(fd, split, serv);
	else
		squery_from_client(fd, split, serv);
}
