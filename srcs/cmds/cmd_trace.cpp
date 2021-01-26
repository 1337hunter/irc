#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"

void	trace_from_network(int fd, t_strvect const &split, IRCserv *serv)
{
	Client		*client;
	t_server	*_serv;
	size_t		c;
	std::MAP<int, t_fd>::iterator fds;

	if (split.size() != 3)
		return ;
	if (!(client = find_client_by_nick(get_nick_from_info(split[0]), serv)))
		return ;
	if (match(serv->servername, split[2]))
	{
		serv->fds[fd].wrbuf += get_reply(serv, RPL_TRACELINK, client, std::string("Link ") +
			VERSIONSTRING + (DEBUG_MODE ? " debug " : " ") + serv->servername + " " +
			serv->servername + " " + "V" + std::string(VERSION) + " " +
			TOSTRING(ft_getcurrenttime() - serv->dtstarted) + " " + "0 0", "");
		c = 0;
		fds = serv->fds.begin();
		for (; fds != serv->fds.end(); fds++)
			if (fds->second.type == FD_SERVICE)
				c++;
		serv->fds[fd].wrbuf += get_reply(serv, RPL_TRACESERVER, client, "Serv " +
				TOSTRING(c) + "S " + TOSTRING(serv->clients.size()) +
				"C " + serv->servername, "");
		serv->fds[fd].wrbuf += get_reply(serv, RPL_TRACEEND, client, serv->servername  +
		" " + VERSIONSTRING + (DEBUG_MODE ? " debug" : ""), "End of TRACE");
	}
	else
	{
		if (!(_serv = find_server_by_mask(split[2], serv)))
			return ;
		serv->fds[fd].wrbuf += get_reply(serv, RPL_TRACELINK, client, std::string("Link ") +
			VERSIONSTRING + (DEBUG_MODE ? " debug " : " ") + serv->servername + " " +
			get_servername_by_mask(split[2], serv) + " " +
			"V" + std::string(VERSION) + " " +
			TOSTRING(ft_getcurrenttime() - serv->dtstarted) + " " +
			TOSTRING(serv->fds[_serv->fd].recvbytes) + " " +
			TOSTRING(serv->fds[_serv->fd].sentbytes), "");
		c = 0;
		fds = serv->fds.begin();
        for (; fds != serv->fds.end(); fds++)
			if (fds->second.type == FD_SERVICE)
				c++;
		serv->fds[fd].wrbuf += get_reply(serv, RPL_TRACESERVER, client, "Serv " +
				TOSTRING(c) + "S " + TOSTRING(serv->clients.size()) +
				"C " + serv->servername, "");
		serv->fds[_serv->fd].wrbuf += strvect_to_string(split) + CRLF;
	}
}

void	trace_from_client(int fd, t_strvect const &split, IRCserv *serv)
{
	Client		*client;
	Client		*oper;
	t_server	*_serv = 0;
	int		c;
	std::list<t_server>::iterator	net;
	std::MAP<int, t_fd>::iterator	fds;
	std::list<t_service>::iterator	service;


	if (!(client = find_client_by_fd(fd, serv)) || !client->isRegistred())
	{
		serv->fds[fd].wrbuf += get_reply(serv, "451", -1, "", "You have not registered");
		return ;
	}
	if (split.size() == 1 && client->isOperator())
	{
		fds = serv->fds.begin();
		for (; fds != serv->fds.end(); fds++)
			if (fds->second.type == FD_OPER)
			{
				if (!(oper = find_client_by_fd(fds->first, serv)))
					continue ;
				else
					serv->fds[fd].wrbuf += get_reply(serv, RPL_TRACEOPERATOR, client,
							"Oper 10 " + oper->getnick(), "");
			}
			else if (fds->second.type == FD_CLIENT)
			{
				if (!(oper = find_client_by_fd(fds->first, serv)))
					continue ;
				else
					serv->fds[fd].wrbuf += get_reply(serv, RPL_TRACEUSER, client,
							"User 10 " + oper->getnick(), "");
			}
			else if (fds->second.type == FD_UNREGISTRED)
				serv->fds[fd].wrbuf += get_reply(serv, RPL_TRACEUNKNOWN, client,
						"???? -3 [" + fds->second.hostname + "]", "");
			else if (fds->second.type == FD_SERVICE)
			{
				service = serv->services.begin();
				for (; service != serv->services.end(); service++)
					if (service->fd == fds->first)
						break ;
				if (service != serv->services.end())
				{
					serv->fds[fd].wrbuf += get_reply(serv, RPL_TRACESERVICE, client,
							"10 " + service->name + " " + service->type + " " +
							service->info, "");
				}
			}
		net = serv->network.begin();
		for (; net != serv->network.end(); net++)
		{
			c = 0;
			service = serv->services.begin();
			for ( ; service != serv->services.end(); service++)
				if (service->fd != -1 && match(net->servername, service->distribution))
					c++;
			serv->fds[fd].wrbuf += get_reply(serv, RPL_TRACESERVER, client, "Serv " +
					TOSTRING(c) + "S " + TOSTRING(net->clients.size()) +
					"C " + net->servername, "");
		}
	}
	else if (split.size() == 2)
	{
		if (!(_serv = find_server_by_mask(split[1], serv)) &&
				!match(serv->servername, split[1]))
		{
			serv->fds[fd].wrbuf += get_reply(serv, "402", client, split[1],
					"No such server"); return ;
		}
		serv->fds[fd].wrbuf += get_reply(serv, RPL_TRACELINK, client, std::string("Link ") +
				VERSIONSTRING + (DEBUG_MODE ? " debug " : " ") + serv->servername + " " +
				(_serv ? get_servername_by_mask(split[1], serv) : serv->servername) + " " +
				"V" + std::string(VERSION) + " " +
				TOSTRING(ft_getcurrenttime() - serv->dtstarted) + " " +
				(_serv ? TOSTRING(serv->fds[_serv->fd].recvbytes) + " " +
				 TOSTRING(serv->fds[_serv->fd].sentbytes) : "0 0") , "");
		c = 0;
		fds = serv->fds.begin();
		for (; fds != serv->fds.end(); fds++)
			if (fds->second.type == FD_SERVICE)
				c++;
		serv->fds[fd].wrbuf += get_reply(serv, RPL_TRACESERVER, client, "Serv " +
				TOSTRING(c) + "S " + TOSTRING(serv->clients.size()) +
				"C " + serv->servername, "");
		if (_serv)
			serv->fds[_serv->fd].wrbuf += ":" + client->getnick() + " " +
			strvect_to_string(split) + CRLF;
	}
	if (!_serv)
		serv->fds[fd].wrbuf += get_reply(serv, RPL_TRACEEND, client, serv->servername  +
		" " + VERSIONSTRING + (DEBUG_MODE ? " debug" : ""), "End of TRACE");
}

void	cmd_trace(int fd, t_strvect const &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_SERVER)
		trace_from_network(fd, split, serv);
	else
		trace_from_client(fd, split, serv);
}
