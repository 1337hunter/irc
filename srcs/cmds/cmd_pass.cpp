#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"
#include "message.hpp"

void		cmd_pass(int fd, const t_strvect &split, IRCserv *serv)
{
	size_t	pos = 0;
	if (split.size() < 2)
	{
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
			ERR_NEEDMOREPARAMS, "", split[0], "Not enough parameters");
		return ;
	}
	if (serv->fds[fd].type != FD_UNREGISTRED && serv->fds[fd].type != FD_SERVER)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_ALREADYREGISTRED, fd, "",
				"Unauthorized command (already registered)"); return ;
	}
	serv->fds[fd].pass = split[1];
	if (split.size() == 2)
		return ;
	serv->fds[fd].version = split[2];
	if (split.size() == 3)
		return ;
	serv->fds[fd].flags = split[3];
	pos = split[3].find_first_of("|");
	if (pos == NPOS)
	{
		serv->fds[fd].wrbuf += "ERROR :Bad options!";
		serv->fds[fd].status = false;
	}
	pos++;
	if (!split[3].compare(pos, 5, "DEBUG"))
	{
		if (!DEBUG_MODE)
		{
			serv->fds[fd].wrbuf += "ERROR :This server is not runing in debug mode!";
			serv->fds[fd].status = false;
		}
	}

	if (split.size() == 4)
		return ;
	serv->fds[fd].options = split[4];
}
