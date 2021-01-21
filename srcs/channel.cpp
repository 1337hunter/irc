#include "channel.hpp"
#include "tools.hpp"

std::string	get_safe_postfix(void)
{
	time_t time = ft_getcurrenttime();
	std::string	postfix;
	int			index;

	index = 0;
	while (time > 0)
	{
		index = time % 36;
		time = time - time % 36;
		if (index < 26)
			postfix += static_cast<char>('A' + index);
		else if (index < 35)
			postfix += static_cast<char>('0' + index - 25);
		else
			postfix += "0";
	}
	return postfix;
}

channel_flags::channel_flags(void) : _anonymous(false), _invite_only(false),
	_moderated(false), _no_messages_outside(true), _quiet(false), _private(false),
	_secret(false), _reop(false), _topic_settable_by_chop(true), _key(""),
	_limit_of_users(NPOS) {}

channel_flags::channel_flags(const channel_flags &o)
{
	*this = o;
}

channel_flags	&channel_flags::operator=(const channel_flags &o)
{
	_anonymous = o._anonymous;
	_invite_only = o._invite_only;
	_moderated = o._moderated;
	_no_messages_outside = o._no_messages_outside;
	_quiet = o._quiet;
	_private = o._private;
	_secret = o._secret;
	_reop = o._reop;
	_topic_settable_by_chop = o._topic_settable_by_chop;
	_key = o._key;
	_limit_of_users = o._limit_of_users;
	_ban_mask = o._ban_mask;
	_exception_mask = o._exception_mask;
	_Invitation_mask = o._Invitation_mask;
	return *this;
}

channel_flags::~channel_flags(void) {}

client_flags::client_flags(bool Operator, bool oper, bool voice) :
	_Operator(Operator), _operator(oper), _voice(voice) {}

client_flags::client_flags(bool *mode) : _Operator(mode[0]),
	_operator(mode[1]), _voice(mode[2]) {}

client_flags::client_flags(client_flags const &obj)
{
	_Operator = obj._Operator;
	_operator = obj._operator;
	_voice = obj._voice;
}

client_flags	&client_flags::operator=(client_flags const &obj)
{
	_Operator = obj._Operator;
	_operator = obj._operator;
	_voice = obj._voice;
	return *this;
}

client_flags::client_flags(void) : _Operator(0), _operator(0), _voice(0) {}

Channel::Channel(std::string const &name) : _name(name), _blocked(false),
	_creation_time(ft_getcurrenttime())
{
	if (name[0] == '!')
		_safe_postfix = get_safe_postfix();
	_topic = "";
}

Channel::Channel(std::string const &name, Client *client) : _name(name), _blocked(false),
	_creation_time(ft_getcurrenttime())
{
	if (name.size() > 0 && name[0] == '#')
	{
		_clients[client] = client_flags(1, 1, 0);
		_channel_creator = client->getinfo();
	}
	else if (name.size() > 0 && name[0] == '+')
		_clients[client];
	else if (name.size() > 0 && name[0] == '!')
	{
		_safe_postfix = get_safe_postfix();
		_clients[client] = client_flags(1, 1, 0);
		_channel_creator = client->getinfo();
	}
	_topic = "";
}

Channel::Channel(std::string const &name, Client *client, std::string const &modes) :
	_name(name), _blocked(0), _creation_time(ft_getcurrenttime())
{
	bool	bmodes[3];

	bmodes[0] = 0;
	bmodes[1] = 0;
	bmodes[2] = 0;
	for (size_t	i = 0; i < modes.size(); i++)
		if (modes[i] == 'O')
		{
			bmodes[0] = 1;
			_channel_creator = client->getinfo();
		}
		else if (modes[i] == 'o')
			bmodes[1] = 1;
		else if (modes[i] == 'v')
			bmodes[2] = 1;
	_clients[client] = client_flags(bmodes);
	_topic = "";
}


Channel::~Channel() {}

Channel::Channel(Channel const &other)
{
	*this = other;
}

Channel::Channel(std::string const &name, std::string const &key, Client *client) :
	_name(name), _blocked(0), _creation_time(ft_getcurrenttime())
{
	_flags._key = key;
	if (name.size() > 0 && name[0] == '#')
		_clients[client] = client_flags(1, 1, 0);
	else if (name.size() > 0 && name[0] == '+')
		_clients[client];
	else if (name.size() > 0 && name[0] == '!')
	{
		_safe_postfix = get_safe_postfix();
		_clients[client] = client_flags(1, 1, 0);
	}
	else if (name.size() > 0 && name[0] == '&')
		_clients[client] = client_flags(1, 1, 0);
}

Channel	&Channel::operator=(Channel const &o)
{
	_name = o._name;
	_blocked = o._blocked;
	_creation_time = o._creation_time;
	_reop_delay = o._reop_delay;
	_safe_postfix = o._safe_postfix;
	_topic = o._topic;
	_flags = o._flags;
	_clients = o._clients;
	return *this;
}

std::MAP<Client*, client_flags> &Channel::getclients(void)
{
	return _clients;
}

std::string	const &Channel::getname(void)
{
	return this->_name;
}

std::string	const &Channel::getkey(void)
{
	return _flags._key;
}


std::string		Channel::getCreationTime(void)
{
	return TOSTRING(_creation_time);
}

std::string const &Channel::getCreator(void)
{
	return _channel_creator;
}

void		Channel::add_client(Client *client)
{
	_clients[client];
}

void		Channel::add_client(Client *client, bool O, bool o, bool v)
{
	if (O)
	{
		_clients[client] = client_flags(1, 1, v);
		_channel_creator = client->getinfo();
	}
	else
		_clients[client] = client_flags(0, o, v);
}

bool		Channel::isSecret(void)
{
	return _flags._secret;
}

bool		Channel::isPrivate(void)
{
	return _flags._private;
}

bool		Channel::isOperator(Client *client)
{
	return _clients[client]._Operator || _clients[client]._operator;
}

std::string	const &Channel::gettopic(void)
{
	return _topic;
}

void		Channel::settopic(std::string const &topic)
{
	_topic = topic;
}

int	Channel::setMode(std::string const &mode)
{
	bool	set;
	size_t	pos;
	std::string	reply;

	if (mode.size() > 0 && mode[0] == '+')
		set = true;
	else if (mode.size() > 0 && mode[0] == '-')
		set = false;
	else
		return true;
	pos = 0;
	while (++pos < mode.size())
	{
		if (mode[pos] == 'a')
			_flags._anonymous = set;
		else if (mode[pos] == 'i')
			_flags._invite_only = set;
		else if (mode[pos] == 'm')
			_flags._moderated = set;
		else if (mode[pos] == 'n')
			_flags._no_messages_outside = set;
		else if (mode[pos] == 'q')
			_flags._quiet = set;
		else if (mode[pos] == 'p')
			_flags._private = set;
		else if (mode[pos] == 'r')
			_flags._reop = set;
		else if (mode[pos] == 't')
			_flags._topic_settable_by_chop = set;
		else if (mode[pos] == 's')
			_flags._secret = set;
		else if (mode[pos] == 'l' && !set)
			_flags._limit_of_users = NPOS;
		else
			return INT_ERR_NEEDMOREPARAMS;
	}
	return 0;
}

int	Channel::setMode(t_strvect const &args)
{
	size_t	pos;
	size_t	i;
	size_t	n;
	std::vector<Client*>		clients;
	std::vector<std::string>	mode;
	std::vector<std::string>	arg;
	std::string					strmode;
	std::list<std::string>::iterator	it;
	std::MAP<Client*, client_flags>::iterator client;

	i = 0;
	while (i < args.size())
	{
		if (args[i][0] != '-' && args[i][0] != '+')
			return INT_ERR_NEEDMOREPARAMS;
		pos = 0;
		n = 0;
		while (++pos < args[i].size())
		{
			if (args[i][pos] == 'k' && args[i][0] == '+' && !_flags._key.empty())
				return INT_ERR_KEYSET;
			if (args[i][pos] == 'I' || args[i][pos] == 'e' || args[i][pos] == 'b' ||
					args[i][pos] == 'k' || args[i][pos] == 'l' || args[i][pos] == 'v' ||
					args[i][pos] == 'o' || args[i][pos] == 'O')
			{
				++n;
				if ((args.size() <= i + n || args[i + n][0] == '+' ||
						args[i + n][0] == '-') && i < args.size())
					return INT_ERR_NEEDMOREPARAMS;
				strmode += args[i][0];
				strmode += args[i][pos];
				if (args[i][pos] == 'o' || args[i][pos] == 'O' || args[i][pos] == 'v')
				{
					for (client = _clients.begin(); client != _clients.end(); client++)
						if (client->first->getnick() == args[i + n])
							break ;
					if (client == _clients.end())
						return 1;
					else
						clients.push_back(client->first);
				}
				mode.push_back(strmode);
				arg.push_back(args[i + n]);
			}
			else
			{
				strmode += args[i][0];
				strmode += args[i][pos];
				mode.push_back(strmode);
				arg.push_back("");
			}
			strmode.erase();
		}
		i += n + 1;
	}
	n = 0;
	i = -1;
	while (++i < mode.size())
	{
		if (mode[i].find_first_not_of("+-aimstrnqp") == NPOS)
			setMode(mode[i]);
		else
		{
			if (mode[i][1] == 'I')
			{
				it = std::find(_flags._Invitation_mask.begin(),
					 _flags._Invitation_mask.end(), arg[i]);
				if (mode[i][0] == '+')
				{
					if (it == _flags._Invitation_mask.end())
						_flags._Invitation_mask.push_back(arg[i]);
				}
				else
					if (it != _flags._Invitation_mask.end())
						_flags._Invitation_mask.erase(it);
			}
			else if (mode[i][1] == 'b')
			{
				it = std::find(_flags._ban_mask.begin(),
						 _flags._ban_mask.end(), arg[i]);
				if (mode[i][0] == '+')
				{
					if (it == _flags._ban_mask.end())
						_flags._ban_mask.push_back(arg[i]);
				}
				else
					if (it != _flags._ban_mask.end())
						_flags._ban_mask.erase(it);
			}
			else if (mode[i][1] == 'e')
			{
				it = std::find(_flags._exception_mask.begin(),
					 _flags._exception_mask.end(), arg[i]);
				if (mode[i][0] == '+')
				{
					if (it == _flags._exception_mask.end())
						_flags._exception_mask.push_back(arg[i]);
				}
				else
					if (it != _flags._exception_mask.end())
						_flags._exception_mask.erase(it);
			}
			else if (mode[i][1] == 'k')
			{
				if (mode[i][0] == '+')
					_flags._key = arg[i];
				if (mode[i][0] == '-' && arg[i] == _flags._key)
					_flags._key.erase();
			}
			else if (mode[i][1] == 'v')
			{
				if (mode[i][0] == '+')
					_clients[clients[n]]._voice = true;
				else
					_clients[clients[n]]._voice = false;
				++n;
			}
			else if (mode[i][1] == 'o')
			{
				if (mode[i][0] == '+')
					_clients[clients[n]]._operator = true;
				else
					_clients[clients[n]]._operator = false;
				++n;
			}
			else if (mode[i][1] == 'O')
			{
				if (mode[i][0] == '+')
					_clients[clients[n]]._Operator = true;
				else
					_clients[clients[n]]._Operator = false;
				n++;
			}
		}
	}
	return 0;
}

channel_flags const &Channel::getflags(void)
{
	return _flags;
}

std::string	Channel::getMode(void)
{
	std::string	mode;

	mode += '+';
	if (_flags._anonymous)
		mode += 'a';
	if (_flags._invite_only)
		mode += 'i';
	if (_flags._moderated)
		mode += 'm';
	if (_flags._no_messages_outside)
		mode += 'n';
	if (_flags._quiet)
		mode += 'q';
	if (_flags._private)
		mode += 'p';
	if (_flags._secret)
		mode += 's';
	if (_flags._reop)
		mode += 'r';
	if (_flags._topic_settable_by_chop)
		mode += 't';
	return mode;
}

Channel		*Channel::getptr(void)
{
	Channel	*ptr = (Channel*)this;
	return ptr;
}

void		Channel::block(void)
{
	_blocked = true;
	_blocked_time = ft_getcurrenttime();
}

void		Channel::unblock(void)
{
	_blocked = false;
}

bool		Channel::isBlocked(void)
{
	return _blocked;
}

time_t		Channel::getBlockedTime(void)
{
	return _blocked_time;
}

bool		Channel::isOnChan(Client *client)
{
	std::MAP<Client*, client_flags>::iterator	it;

	for (it = _clients.begin(); it != _clients.end(); it++)
		if (it->first == client)
			return true;
	return false;
}

bool		Channel::isInvited(Client *client)
{
	return client->isInvited(this);
}

bool		Channel::isBanned(Client *client)
{
	return client->isBanned(this);
}

bool		Channel::block_if(void)
{
	std::MAP<Client*, client_flags>::iterator	client;

	if (_blocked)
		return false;
	client = _clients.begin();
	for (; client != _clients.end(); client++)
		if (!client->first->isBlocked())
			return false;
	this->block();
	return true;
}

std::string	Channel::size(void)
{
	return TOSTRING(_clients.size());
}
