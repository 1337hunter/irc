
#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"

void	cmd_squery(int fd, const t_strvect &split, IRCserv *serv)
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
	serv->fds[service->fd].wrbuf += split[2] + CRLF;
#if DEBUG_MODE
	std::cout << "client " << fd <<
		" sent service query to " << split[1] <<
		" service:" << std::endl << split[2] << std::endl;
#endif

}
