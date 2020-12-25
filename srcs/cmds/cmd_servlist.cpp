#include "ircserv.hpp"
#include "message.hpp"
#include "tools.hpp"

void	cmd_servlist(int fd, t_strvect const &split, IRCserv *serv)
{
	Client  *client;
	std::list<t_service>::iterator  service;

	if (serv->fds[fd].type == FD_SERVER)
		return ;
	if (!(client = find_client_by_fd(fd, serv)) || !client->isRegistred())
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTREGISTERED, -1, "",
			"You have not registered"); return ;
	}
	if (split.size() == 1)
	{
		service = serv->services.begin();
		for (; service != serv->services.end(); service++)
			if (service->fd != -1)
			{
				serv->fds[fd].wrbuf += get_reply(serv, RPL_SERVLIST, client,
				service->name + " " + get_servername_by_token(service->token, serv) + " " +
				service->distribution + " " + service->type + " " +
				get_hopcount_by_token(service->token, serv) + " " + service->info, "");
			}
	}
	else if (split.size() == 2)
	{
		service = serv->services.begin();
		for (; service != serv->services.end(); service++)
		{
			if (service->fd != -1 && match(service->name, split[1]))
			{
				serv->fds[fd].wrbuf += get_reply(serv, RPL_SERVLIST, client,
				service->name + " " + get_servername_by_token(service->token, serv) + " " +
				service->distribution + " " + service->type + " " +
				get_hopcount_by_token(service->token, serv) + " " + service->info, "");

			}
		}
	}
	else if (split.size() == 3)
	{
		service = serv->services.begin();
		for (; service != serv->services.end(); service++)
		{
			if (service->fd != -1 && match(service->name, split[1]) &&
					service->type == split[2])
			{
				serv->fds[fd].wrbuf += get_reply(serv, RPL_SERVLIST, client,
				service->name + " " + get_servername_by_token(service->token, serv) + " " +
				service->distribution + " " + service->type + " " +
				get_hopcount_by_token(service->token, serv) + " " + service->info, "");

			}
		}
	}
	serv->fds[fd].wrbuf += get_reply(serv, RPL_SERVLISTEND, client, "",
			"End of service listing");
}
