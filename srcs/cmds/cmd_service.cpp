#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"

void	service_from_network(int fd, t_strvect const &split, IRCserv *serv)
{
	t_service	*service;
	t_service	temp;
	t_strvect   cmd;

	if (split.size() < 8)
		return ;
	if (!match(serv->servername, split[4]))
			return ;
	cmd = split;
	if (!(service = find_service_by_name(split[2], serv)))
	{
		temp.fd = fd;
		try { temp.hopcount = STOI(split[6]); } catch(...) { return ; }
		temp.name = split[2];
		temp.distribution = split[4];
		temp.type = split[5];
		temp.info = split[7];
		temp.token = split[3];
		cmd[6] = TOSTRING(temp.hopcount + 1);
		serv->services.push_back(temp);
	}
	else
	{
		try { service->hopcount = STOI(split[6]); } catch(...) { return ; }
		service->distribution = split[4];
		service->type = split[5];
		service->info = split[7];
		service->token = split[3];
		service->fd = fd;
		cmd[6] = TOSTRING(service->hopcount + 1);
	}
	cmd[7] = ":" + cmd[7];
	msg_forward(fd, strvect_to_string(cmd), serv);
}

void	service_from_service(int fd, t_strvect const &split, IRCserv *serv)
{
	t_service	*service;
	std::string	forward;

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
	if (serv->fds[fd].pass != service->pass)
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
	forward = ":" + serv->servername + " SERVICE " + service->name + " " + serv->token +
		" " + service->distribution + " " + service->type + " 1 :" +
		strvect_to_string(split, ' ', 6);
	msg_forward(fd, forward, serv);
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
		service_from_network(fd, split, serv);
	else
		service_from_service(fd, split, serv);
}
