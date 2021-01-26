#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"

void		cmd_pong(int fd, const t_strvect &split, IRCserv *serv)
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

	t_strvect	args(split.begin() + i + 1, split.end());
	if (args.size() < 1)
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NOORIGIN, "", "", "No origin specified");
	else if ((args.size() == 1 && args[0] == serv->servername) ||
		(args.size() >= 2 && args[1] == serv->servername))
	{
		fdref.awaitingpong = false;
		fdref.lastactive = ft_getcurrenttime();
	}
	else if (args.size() >= 2)	// then forward to args[2]
	{
		Client		*client = NULL;
		t_service	*service = NULL;
		if (nick.empty() && (client = find_client_by_fd(fd, serv)))
			nick = client->getnick();
		else if (nick.empty() && (service = find_service_by_fd(fd, serv)))
			nick = service->name;
		if (nick.empty() && (fdref.type == FD_CLIENT || fdref.type == FD_OPER ||
			fdref.type == FD_SERVICE))
			return ;

		t_server	*sendto = find_server_by_name(args[1], serv);
		if (sendto)
			serv->fds[sendto->fd].wrbuf += ":" + nick + " PONG " +
				args[0] + " :" + args[1] + CRLF;
		else
		{
			client = find_client_by_nick(args[1], serv);
			if (client)
				serv->fds[client->getFD()].wrbuf += ":" + nick + " PONG " +
					args[0] + " :" + args[1] + CRLF;
			else
				fdref.wrbuf += ft_buildmsg(serv->servername,
					ERR_NOSUCHSERVER, nick, args[1], "No such server");
		}
	}
}
