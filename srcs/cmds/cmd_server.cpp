#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

//split: :<behind> SERVER <servername> <hopcount> <token> <info>
void	introduce_server_behind(int fd, const t_strvect &split, IRCserv *serv)
{
	std::vector<t_server>::iterator begin = serv->network.begin();
	t_server_intro	temp;
	std::string		behind(split[0], 1);
	t_strvect		forward_vect;
	std::string		forward;

	while (begin != serv->network.end() && begin->fd != fd)
		begin++;
	if (begin == serv->network.end()) //error
		return ;
	try { temp.hopcount = stoi(split[3]); } catch (std::exception &e) { (void)e; return; }
	temp.servername = split[2];
	temp.behind = behind;
	temp.info = strvect_to_string(split, ' ', 5);
	temp.token = split[3];
	begin->routing.push_back(temp);
	// forward broadcast
	forward_vect = split;
	forward_vect[3] = std::to_string(temp.hopcount + 1);
	forward = strvect_to_string(forward_vect);
	forward += CRLF;
	begin = serv->network.begin();
	for (; begin != serv->network.end(); begin++)
		if (begin->fd != fd)
			serv->fds[begin->fd].wrbuf += forward;
}

bool	send_clients_and_channels(int fd, IRCserv *serv)
{
	std::list<Client>::iterator		client;
	std::vector<t_server>::iterator	net;
	std::list<Channel>::iterator	channel;
	std::string						enjoy;
	std::string						channel_forward;
	std::list<std::string>::const_iterator				Musk;
	std::unordered_map<Client*, client_flags>::iterator	client_chan;

	for (client = serv->clients.begin(); client != serv->clients.end(); client++)
	{
		serv->fds[fd].wrbuf += "NICK " + client->getnick() + " 1 " +
			client->getusername() + " " + client->gethostname() + " " +
			serv->token + client->getMode(true) + ":" + client->getrealname() + CRLF;
	}
	for (net = serv->network.begin(); net != serv->network.end(); net++)
	{
		for (client = net->clients.begin(); client != net->clients.end(); client++)
		{
			serv->fds[fd].wrbuf += "NICK " + client->getnick() +
				client->gethopcount(true, true) + client->getusername() + " " +
				client->gethostname() +
				" " + client->gettoken() + client->getMode(true) +
				":" + client->getrealname() + CRLF;
		}
	}
	channel = serv->channels.begin();
	for (; channel != serv->channels.end(); channel++)
	{
		if (channel->getname()[0] == '&')
			continue ;
		client_chan = channel->getclients().begin();
		for (; client_chan != channel->getclients().end(); client_chan++)
		{
			if (client_chan->second._Operator)
				enjoy += "@@";
			else if (client_chan->second._operator)
				enjoy += "@";
			else if (client_chan->second._voice)
				enjoy += "+";
			enjoy += client_chan->first->getnick() + ",";
		}
		if (enjoy.size() > 0)
		{
			enjoy.pop_back();
			channel_forward = ":" + serv->servername + " NJOIN " + channel->getname() +
				" :" + enjoy + CRLF;
			enjoy.erase();
			serv->fds[fd].wrbuf += channel_forward;
		}
		if (channel->getname()[0] == '+')
			continue ;
		serv->fds[fd].wrbuf += ":" + serv->servername + " MODE " + channel->getname() +
			" " + channel->getMode() + CRLF;
		if (!channel->getflags()._key.empty())
			serv->fds[fd].wrbuf += ":" + serv->servername + " MODE " + channel->getname() +
			" +k " + channel->getflags()._key + CRLF;
		if (channel->getflags()._ban_mask.size() > 0)
		{
			Musk = channel->getflags()._ban_mask.begin();
			for (; Musk != channel->getflags()._ban_mask.end(); Musk++)
				serv->fds[fd].wrbuf += ":" + serv->servername + " MODE " +
				channel->getname() + " +b " + *Musk + CRLF;
		}
		if (channel->getflags()._exception_mask.size() > 0)
		{
			Musk = channel->getflags()._exception_mask.begin();
			for (; Musk != channel->getflags()._exception_mask.end(); Musk++)
				serv->fds[fd].wrbuf += ":" + serv->servername + " MODE " +
				channel->getname() + " +e " + *Musk + CRLF;
		}
		if (channel->getflags()._Invitation_mask.size() > 0)
		{
			Musk = channel->getflags()._Invitation_mask.begin();
			for (; Musk != channel->getflags()._Invitation_mask.end(); Musk++)
				serv->fds[fd].wrbuf += ":" + serv->servername + " MODE " +
				channel->getname() + " +I " + *Musk + CRLF;
		}
	}
	return false;
}

//split: SERVER <servername> <hopcount> <token> <info>
void	cmd_server(int fd, const t_strvect &split, IRCserv *serv)
{
	t_server	temp;
	std::vector<t_link>::iterator	link;

	if ((split.size() < 4 && split[0] == "SERVER") ||
			(split.size() < 5 && split[0][0] == ':'))
	{
		serv->fds[fd].wrbuf += "ERROR :Not enough SERVER parameters\r\n";
		serv->fds[fd].status = false;
		return ;
	}
	if (split[0][0] == ':')
	{
		clear_block_list(serv, std::string(split[0], 1));
		introduce_server_behind(fd, split, serv);
		return ;
	}
	clear_block_list(serv, split[1]);
	link = serv->link.begin();
	// looking for link with servername (is SERVER servername allowd to connect to us?)
	while (link != serv->link.end())
	{
		if (link->servername == split[1])
			break ;
		link++;
	}
	if (link == serv->link.end())
	{
		serv->fds[fd].wrbuf += "ERROR :You are not allowed to connect to.";
		serv->fds[fd].wrbuf += CRLF;
		serv->fds[fd].status = false;
		return ;
	}
	if (is_server_registred(split[1], split[3], serv))
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_ALREADYREGISTRED, -1, "",
		split[1] + " servername or token is already registred");
		serv->fds[fd].status = false;
		return ;
	}
	if (!(serv->fds[fd].pass == serv->pass || serv->pass == ""))
	{
		serv->fds[fd].wrbuf += "ERROR :Password incorrect";
		serv->fds[fd].status = false; return ;
	}
	//backward message to introduce us to enother server
	if (serv->fds[fd].type != FD_SERVER)
	{
		serv->fds[fd].wrbuf += "PASS " + link->pass + " " + VERSION + " " + "IRC|\r\n";
		serv->fds[fd].wrbuf += "SERVER " + serv->servername + " 1 " + serv->token +
		" :" + serv->info + CRLF;
	}
	temp.servername = split[1];
	try
	{
		temp.hopcount = stoi(split[2]);
		temp.token = split.size() < 5 ? std::to_string(NPOS) : split[3];
	}
	catch (std::exception &e)
	{
		msg_error("Bad hopcount. Connection is terminated.", serv);
		cmd_squit(fd, split, serv);
		return ;
	}
	if (temp.hopcount == 1)
		temp.fd = fd;
	else
	{
		msg_error("Error while SERVER message process. Connection terminated.", serv);
		cmd_squit(fd, split, serv);
		return ;
	}
	temp.info = split.size() < 5 ? split[3] : split[4];
	msg_forward(fd, ":" + split[1] + " SERVER " + serv->servername + " 2 " +
			temp.token + " " + temp.info , serv);
	std::vector<t_server>::iterator begin = serv->network.begin();
	std::vector<t_server>::iterator end = serv->network.end();
	std::string	backward;
	std::list<t_server_intro>::iterator	serv_intro;
	begin = serv->network.begin();
	while (begin != end)
	{
		backward += ":" + begin->servername + " SERVER " + serv->servername +
			" 2 " + begin->token + " " + begin->info + CRLF; //send nearest servers
		serv_intro = begin->routing.begin();
		while (serv_intro != begin->routing.end())
		{
			backward += ":" + serv_intro->behind + " SERVER " + serv_intro->servername +
				" " + std::to_string(serv_intro->hopcount + 1) + " " +
				serv_intro->token + " " + serv_intro->info + CRLF;
			serv_intro++;
		}
		begin++;
	}
	send_clients_and_channels(fd, serv);
	serv->fds[fd].wrbuf += backward;
	serv->fds[fd].type = FD_SERVER;
	if (serv->fds[fd].linkname.size() > 0 && serv->fds[fd].linkname[0] == '*')
		serv->fds[fd].linkname = temp.servername + serv->fds[fd].linkname.substr(1);
	serv->network.push_back(temp);
}
