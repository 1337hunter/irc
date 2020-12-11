#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"

//Command: PRIVMSG
//Parameters: <receiver>{,<receiver>} <text to be sent>
//

void	privmsg_from_network(int fd, t_strvect const &split, IRCserv *serv)
{
	fd = 0; (void)split; serv = 0;
}

void	privmsg_from_client(int fd, t_strvect const &split, IRCserv *serv)
{
	Client	*client;
//	Channel	*channel;

	if (!(client = find_client_by_fd(fd, serv)) || !client->isRegistred())
	{
		serv->fds[fd].wrbuf += get_reply(serv, "451", -1, "", "You have not registered");
		return ;
	}
	if (split.size() < 2)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NORECIPIENT, client, "",
				"No recipient given (PRIVMSG)"); return ;
	}
	if (split.size() < 3)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOTEXTTOSEND, client, "",
				"No text to send"); return ;
	}
	if (split[1].find_first_of("*?") != NPOS)
	{
		if (!client->isOperator())
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_NOPRIVILEGES, client, "",
			"Permission Denied- You're not an IRC operator"); return ;
		}
		else if (!is_valid_serv_host_mask(split[1]))
		{
			serv->fds[fd].wrbuf += get_reply(serv, ERR_BADMASK, client, split[1],
					"Bad Server/host mask"); return ;
		}
	}
	if (split[1][0] == '$')
	{
	}
	else if (split[1][0] == '#' || split[1][0] == '!' || split[1][0] == '+' ||
			split[1][0] == '&')
	{

	}
	else
	{
	}
}

void	cmd_privmsg(int fd, t_strvect const &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_SERVER)
		privmsg_from_network(fd, split, serv);
	else
		privmsg_from_client(fd, split, serv);
}
