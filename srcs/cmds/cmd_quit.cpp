#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"
#include "message.hpp"

std::vector<Client*>	get_clients_for_quit_msg(Client *client)
{
	std::list<Channel*>::iterator   client_chan;
	std::vector<Client*>            msg_for;
	std::MAP<Client*, client_flags>::iterator chan_client;

	client_chan = client->getchannels().begin();
	for (; client_chan != client->getchannels().end(); client_chan++)
	{
		chan_client = (*client_chan)->getclients().begin();
		for (; chan_client != (*client_chan)->getclients().end(); chan_client++)
			if (chan_client->first->gethop() == 0)
				if (std::find(msg_for.begin(), msg_for.end(), chan_client->first) ==
						msg_for.end())
					msg_for.push_back(chan_client->first);
	}
	return msg_for;
}

int         quit_from_network(int fd, t_strvect const &split, IRCserv *serv)
{
	Client		*client;
	std::string	nick;
	std::vector<Client*>            msg_for;
	std::vector<Client*>::iterator  msg_for_it;

	if (split.size() < 3)
		return 1;
	if (!(client = find_client_by_nick(get_nick_from_info(split[0]), serv)))
		return 1;
	nick = client->getnick();
	msg_for = get_clients_for_quit_msg(client);
	msg_for_it = msg_for.begin();
	for (; msg_for_it != msg_for.end(); msg_for_it++)
		serv->fds[(*msg_for_it)->getFD()].wrbuf += ":" + client->getinfo() + " " +
		strvect_to_string(split, ' ', 1) + CRLF;
	client->partAllChan();
	msg_forward(fd, strvect_to_string(split), serv);
	if (client->gethop() == 0)
	{
		serv->fds[client->getFD()].wrbuf += "ERROR :Closing Link: [" +
			serv->fds[client->getFD()].hostname + "] :" + strvect_to_string(split, ' ', 2)
		   	+ CRLF;
		serv->fds[client->getFD()].status = false;
        serv->fds[client->getFD()].blocked = false;
        serv->fds[client->getFD()].fatal = false;
	}
	remove_client_by_ptr(client, serv);
#if DEBUG_MODE
    std::cout << "nick " << nick << "\t\tdisconnected" << std::endl;
#endif
	return 0;
}


int			quit_from_client(int fd, t_strvect const &split, IRCserv *serv)
{
	t_citer 	it;
	std::string	quit_msg;
	std::vector<Client*>			msg_for;
	std::vector<Client*>::iterator	msg_for_it;

	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (it == serv->clients.end())
	{
		serv->fds[fd].wrbuf += "ERROR :Closing Link: [" + serv->fds[fd].hostname + "]\r\n";
		serv->fds[fd].status = false; return 0;
	}
	if (!it->isRegistred())
	{
		serv->clients.erase(it);
		serv->fds[fd].wrbuf += "ERROR :Closing Link: [" + serv->fds[fd].hostname + "]\r\n";
		serv->fds[fd].status = false;
		serv->fds[fd].blocked = false;
		serv->fds[fd].fatal = false;
		return 0;
	}
	if (split.size() > 1)
		quit_msg = " :" + strvect_to_string(split, ' ', 1);
	else
		quit_msg = " :Default";
	msg_for = get_clients_for_quit_msg(&(*it));
	msg_for_it = msg_for.begin();
	for (; msg_for_it != msg_for.end(); msg_for_it++)
		serv->fds[(*msg_for_it)->getFD()].wrbuf += ":" + it->getinfo() + " QUIT" +
		quit_msg + CRLF;
	it->partAllChan();
#if DEBUG_MODE
	std::cout << "client " << fd << "\t\tdisconnected" << std::endl;
#endif
	msg_forward(fd, ":" + it->getnick() + " QUIT" + quit_msg, serv);
	addtonickhistory(serv, it);
	serv->fds[it->getFD()].wrbuf += "ERROR :Closing Link: [" + serv->fds[fd].hostname +
		"] " + it->getinfo() +
	   	(serv->fds[it->getFD()].awaitingpong ? " (Ping timeout)" : quit_msg) + CRLF;
	serv->fds[it->getFD()].status = false;
	serv->fds[it->getFD()].blocked = false;
	serv->fds[it->getFD()].fatal = false;
	serv->clients.erase(it);
	return (0);
}

void		cmd_quit(int fd, const t_strvect &split, IRCserv *serv)
{
	if (serv->fds[fd].type == FD_SERVER || serv->fds[fd].type == FD_ME)
		quit_from_network(fd, split, serv);
	else
		quit_from_client(fd, split, serv);
}
