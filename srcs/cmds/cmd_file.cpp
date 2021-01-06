#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"

void	cmd_file(int fd, t_strvect const &split, IRCserv *serv)
{
	Client	*client;

	if (split.size() < 4)
	{
		serv->fds[fd].wrbuf += "ERROR :FILE command requires more parameters!";
		return ;
	}
	if (!(client = find_client_by_nick(split[2], serv)))
	{
		serv->fds[fd].wrbuf += get_reply(serv, "401", client, split[1],
				"No such nick/channel"); return ;
	}
	try { serv->fds[fd].file_size = ft_stoi(split[3]); }
	catch (...)
	{
		serv->fds[fd].file_size = 0;
		serv->fds[fd].wrbuf += "ERROR :FILE has errorneous amount of bytes";
		return ;
	}
	for (size_t i = 0; i < split.size() - 1; i++)
		std::cout << split[i];
	for (size_t i = 0; i < split[4].size(); i++)
		std::cout << split[4][i];
}
