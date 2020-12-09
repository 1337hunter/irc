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
	_moderated(false), _no_messages_outside(false), _quiet(false), _private(false),
	_secret(false), _reop(false), _topic_settable_by_chop(false), _key(""),
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
}

Channel	&Channel::operator=(Channel const &o)
{
	_name = o._name;
	_blocked = o._blocked;
	_creation_time = o._creation_time;
	_reop_delay = o._reop_delay;
	_safe_postfix = o._safe_postfix;
	_topic = o._topic;
	_type = o._type;
	_flags = o._flags;
	_clients = o._clients;


	return *this;
}

std::unordered_map<Client*, client_flags> &Channel::getclients(void)
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

char		Channel::gettype(void)
{
	return _type;
}

void		Channel::settype(char type)
{
	_type = type;
}

std::string const &Channel::getCreator(void)
{
	return _channel_creator;
}

void		Channel::add_client(Client *client)
{
	_clients[client];
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
	while (++pos < mode.size()) // unordered_map is better but I did it already
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
	std::vector<std::string>	mode;
	std::vector<std::string>	arg;
	std::string					strmode;
	std::string					strarg;
	std::string					reply;
	std::list<std::string>::iterator	it;

	i = 0;
	while (i < args.size())
	{
		if (args[i][0] != '-' || args[i][0] != '+')
			return INT_ERR_NEEDMOREPARAMS;
		pos = 0;
		n = 0;
		while (++pos < args[i].size())
		{
			if (args[i][pos] == 'I' || args[i][pos] == 'e' || args[i][pos] == 'b' ||
					args[i][pos] == 'k' || args[i][pos] == 'l')
			{
				if (args[i][pos] == 'k' && args[i][0] == '+' && !_flags._key.empty())
					return INT_ERR_KEYSET;
				if (args.size() <= i + pos || args[i + pos][0] == '+' ||
						args[i + pos][0] == '-')
					return INT_ERR_NEEDMOREPARAMS;
				strmode += args[i][0];
				strmode += args[i][pos];
				strarg += args[i + pos];
				mode.push_back(strmode);
				arg.push_back(strarg);
				n++;
			}
			else
			{
				strmode += args[i][0];
				strmode += args[i][pos];
				mode.push_back(strmode);
				arg.push_back("");
			}
		}
		i += n + 1;
	}
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
						_flags._Invitation_mask.erase(it);
			}
			else if (mode[i][1] == 'e')
			{
				it = std::find(_flags._exception_mask.begin(),
					 _flags._exception_mask.end(), arg[i]);
				if (mode[i][0] == '+')
				{
					if (it == _flags._exception_mask.end())
						_flags._ban_mask.push_back(arg[i]);
				}
				else
					if (it != _flags._ban_mask.end())
						_flags._Invitation_mask.erase(it);
			}
			else if (mode[i][1] == 'k')
			{
				if (mode[i][0] == '+')
					_flags._key = arg[i];
				if (mode[i][0] == '-' && arg[i] == _flags._key)
					_flags._key.erase();
			}
		}
	}
	return 0;
}

channel_flags const &Channel::getflags(void)
{
	return _flags;
}

Channel		*Channel::getptr(void)
{
	Channel	*ptr = (Channel*)this;
	return ptr;
}

void		Channel::block(void)
{
	_blocked = true;
}

void		Channel::unblock(void)
{
	_blocked = false;
}

bool		Channel::isBlocked(void)
{
	return this->_blocked;
}

bool		Channel::isOnChan(Client *client)
{
	std::unordered_map<Client*, client_flags>::iterator	it;

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
