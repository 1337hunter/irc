#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"

void		cmd_ping(int fd, const t_strvect &split, IRCserv *serv)
{
	t_fd		&fdref = serv->fds[fd];
	int			i = 0;
	std::string	nick = "";

	if (fdref.type == FD_UNREGISTRED)
		return ;
	if (split[0][0] == ':')
	{
		i = 1;
		nick = split[0].substr(1);
	}
	else if (fdref.type == FD_SERVER)
	{
		t_server	*server = find_server_by_fd(fd, serv);
		if (!server)
			return ;
		nick = server->servername;
	}
	else if (fdref.type == FD_SERVICE)
	{
		t_service	*service = find_service_by_fd(fd, serv);
		if (!service)
			return ;
		nick = service->name;
	}
	else
	{
		Client		*client = find_client_by_fd(fd, serv);
		if (!client)
			return ;
		nick = client->getnick();
	}

	t_strvect	args(split.begin() + i + 1, split.end());
	if (args.size() == 0)
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NOORIGIN, "", "", "No origin specified");
	else if (args.size() == 1 || (args.size() >= 2 && args[1] == serv->servername))
		fdref.wrbuf += ft_buildmsg(serv->servername, "PONG",
			serv->servername, "", args[0]);
	else if (args.size() >= 2)	// then forward to args[1]
	{
		t_server	*sendto = find_server_by_name(args[1], serv);
		if (sendto)
			serv->fds[sendto->fd].wrbuf += ":" + nick + " PING " +
				args[0] + " :" + args[1] + CRLF;
		else
			fdref.wrbuf += ft_buildmsg(serv->servername,
				ERR_NOSUCHSERVER, nick, args[1], "No such server");
	}
}
