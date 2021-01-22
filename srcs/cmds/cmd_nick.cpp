#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"
#include "message.hpp"

//Command: NICK
//Parameters: <nickname>[1] <hopcount>[2] <username>[3] <host>[4] <servertoken>[]
// <umode>[6] <realname>[7]

void	nick_from_network(int fd, const t_strvect &split, IRCserv *serv)
{
	t_server	*routing;
	int			hop;
	Client		*client;
	std::string	forward;
	std::list<t_server>::iterator	net;

	if (split.size() < 8)
		return ;
	if ((client = find_client_by_nick(split[1], serv)))
	{
		serv->fds[fd].wrbuf += "KILL " + split[1] + " :KILL from server " + serv->servername
			+ " (Nickname collision)\r\n";
		serv->fds[fd].wrbuf += ":" + split[1] + " QUIT :KILL from server " +
			serv->servername + " (Nickname collision)\r\n";
		return ;
	}
	try { hop = STOI(split[2]); } catch (std::exception &e) { (void)e; return ; }
	if ((routing = find_server_by_fd(fd, serv)) != 0)
	{
		routing->clients.push_back(Client(split, fd));
		routing->clients.back().setMode(split[6]);
	}
	else
	{
		serv->fds[fd].wrbuf += "ERROR :NICK command from unregistred server\r\n";
		return ;
	}
	forward = "NICK " + split[1] + " " + TOSTRING(hop + 1) + " " + split[3] +
		" " + split[4] + " " + split[5] + " " + split[6] + " :" + split[7];
	for (size_t i = 8; i < split.size(); i++)
		forward += " " + split[i];
	forward += CRLF;
	net = serv->network.begin();
	for (; net != serv->network.end(); net++)
		if (net->fd != fd)
			serv->fds[net->fd].wrbuf += forward;
}

void	nick_from_client(int fd, const t_strvect &split, IRCserv *serv)
{
	Client			*client;
	std::list<t_kill>::iterator	kill;

	if (split.size() < 2)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NONICKNAMEGIVEN, -1, "",
				"No nickname given"); return ;
	}
	if (serv->pass != "" && serv->fds[fd].pass != serv->pass)
    {
		serv->fds[fd].wrbuf += get_reply(serv, ERR_PASSWDMISMATCH, -1, "",
				"Password incorrect"); serv->fds[fd].status = false; return ;
    }
	if (split[1] == "anonymous" || split[1].size() > 9 ||
			split[1].find_first_of("\b\r\n\a!@#$%^&*+-?:\"\',") != NPOS ||
			split[1] == "admin" || split[1] == "oper" ||
			split[1] == "operator" || split[1] == "Operator")
	{
		serv->fds[fd].wrbuf += ":" + serv->servername + " 432 " +
			split[1] + " :Erroneous nickname\r\n"; return ;
	}
	if ((client = find_client_by_nick(split[1], serv)) && client->isRegistred())
	{
		if (client->isBlocked())
			serv->fds[fd].wrbuf += get_reply(serv, ERR_UNAVAILRESOURCE, -1, split[1],
			"Nick/channel is temporarily unavailable");
		else if (client->isRestricted())
			serv->fds[fd].wrbuf += get_reply(serv, ERR_RESTRICTED, -1, "",
			"Your connection is restricted!");
		else if (client->gethop() == 0)
			serv->fds[fd].wrbuf += get_reply(serv, ERR_NICKNAMEINUSE, -1, split[1],
			"Nickname is already in use");
		else if (client->gethop() > 0)
		{
			t_kill	kill;
			kill.nick = client->getnick();
			kill.host = client->gethostname();
			kill.time = ft_getcurrenttime();
			kill.cause = "Nickname collision";
			serv->kills.push_back(kill);
			serv->fds[fd].wrbuf += get_reply(serv, ERR_NICKCOLLISION, -1, split[1],
			"Nickname collision KILL from " + client->getusername() +
			"@" + client->gethostname());
			remove_client_by_ptr(find_client_by_fd(fd, serv), serv);
			serv->fds[fd].status = false;
		}
		return ;
	}
	else if (client && client->gethop() == 0 && !client->isRegistred() &&
			client->getFD() != fd)
	{
		serv->fds[client->getFD()].status = false;
		serv->fds[client->getFD()].wrbuf = "Error :Closing Link: " +
			client->getnick() + " (Overridden)\r\n";
		client->setFD(fd);
		client->sethostname(serv->fds[fd].hostname); return ;
	}
	else if (!client)
	{
		if ((client = find_client_by_fd(fd, serv)) && client->isRegistred())
		{
			t_whowas	whowas;
			serv->fds[fd].wrbuf += ":" + client->getnick() + " NICK " + split[1] + CRLF;
			whowas.nickname = client->getnick();
			whowas.username = client->getusername();
			whowas.realname = client->getrealname();
			whowas.hostname = client->gethostname();
			whowas.servername = serv->servername; whowas.dtloggedin = ft_getcurrenttime();
			serv->nickhistory.push_back(whowas);
			msg_forward(fd, ":" + client->getnick() + " NICK " + split[1], serv);
			std::list<Client>::iterator cit = serv->clients.begin();
			for (; cit != serv->clients.end(); ++cit)
				if (cit->getFD() != client->getFD())
					serv->fds[cit->getFD()].wrbuf += ":" + client->getnick() +
					" NICK " + split[1] + CRLF;
			client->ChangeNick(split[1]); return ;
		}
		else if (client && !client->isRegistred())
		{
			client->sethostname(serv->fds[fd].hostname);
			client->Register(split[1]);
			if (client->getUSER())
			{
				nick_forward(serv, client);
				serv->fds[fd].type = FD_CLIENT;
				serv->fds[fd].wrbuf += reply_welcome(serv, client);
				serv->fds[fd].linkname = client->getnick() + "[" +
					client->getusername() + "@" + client->gethostname() + "]";
			}
			return ;
		}
	}
	for (kill = serv->kills.begin(); kill != serv->kills.end(); kill++)
		if (kill->nick == split[1] || kill->host == serv->fds[fd].hostname)
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_RESTRICTED, -1, "",
			"Your connection is restricted couse " + kill->cause  +
			" for " + TOSTRING(KILLTIME - ft_getcurrenttime() + kill->time) + "s");
			serv->fds[fd].status = false; return ;
		}
	serv->clients.push_back(Client(split[1], serv->token, fd));
	serv->clients.back().sethostname(serv->fds[fd].hostname);
	serv->fds[fd].type = FD_CLIENT;
}

void	cmd_nick(int fd, const t_strvect &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_SERVER)
		nick_from_network(fd, split, serv);
	else
		nick_from_client(fd, split, serv);
}
