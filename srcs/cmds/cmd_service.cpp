#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"

void	service_from_network(int fd, IRCserv *serv)
{
	serv->fds[fd].wrbuf += "ERROR :servers cann't introduce services because standard";
	serv->fds[fd].wrbuf += " 2.10 is not finished. You can only directly connect";
	serv->fds[fd].wrbuf += " to server\r\n";
}

void	service_from_service(int fd, t_strvect const &split, IRCserv *serv)
{
	t_service	*service;

	if (split.size() < 7)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, -1, "SERVICE",
				"Not enough parameters"); return ;
	}
	if (!(service = find_service_by_name(split[1], serv)))
	{
		serv->fds[fd].wrbuf += ":" + serv->servername + " 432 " + split[1] +
			" :Erroneous nickname\r\n"; serv->fds[fd].status = false; return ;
	}
	if (!service->pass.empty() && serv->fds[fd].pass != service->pass)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_PASSWDMISMATCH, -1, "",
			"Password incorrect"); serv->fds[fd].status = false; return ;
	}
	if (!match(serv->servername, service->distribution))
	{
		serv->fds[fd].wrbuf += "ERROR :distribution mismatch. Closing link\r\n";
		serv->fds[fd].status = false; return ;
	}
	if (!match(serv->fds[fd].hostname, service->hostmask))
	{
		serv->fds[fd].wrbuf += "ERROR :hostmask mismatch. Closing link\r\n";
		serv->fds[fd].status = false; return ;
	}
	serv->fds[fd].type = FD_SERVICE;
	service->fd = fd;
	service->hopcount = 0;
	service->token = serv->token;
	serv->fds[fd].wrbuf += get_reply(serv, RPL_YOURESERVICE, -1, "",
		"You are service " + service->name);
	serv->fds[fd].wrbuf += get_reply(serv, RPL_YOURHOST, -1, "", "Your host is " +
		serv->fds[fd].hostname + ", running version " + std::string(VERSION));
	serv->fds[fd].wrbuf += get_reply(serv, RPL_MYINFO, -1, "",
		serv->servername + " " + std::string(VERSION) + " " + serv->usermodes + " " +
		serv->chanmodes);
}

void	cmd_service(int fd, t_strvect const &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_CLIENT || serv->fds[fd].type == FD_OPER)
	{
		serv->fds[fd].wrbuf += "ERROR :This command is allowed to use only for services\r\n";
		serv->fds[fd].status = false;
		return ;
	}
	if (serv->fds[fd].type == FD_SERVICE)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_ALREADYREGISTRED, -1, "",
			"Unauthorized command (already registered)");
		serv->fds[fd].status = false;
		return ;
	}
	if (serv->fds[fd].type == FD_SERVER)
		service_from_network(fd, serv);
	else
		service_from_service(fd, split, serv);
}
