#include "client.hpp"
#include "tools.hpp"

Client::Client() : fd(-1), _isConnected(false), _isRegistred(false),
	USER(false), NICK(false), _isInvisible(false), _isWallOps(false),
	_isServNotice(false), _isOperator(false), _restricted(false), _away(false),
	_away_message(""), _blocked(false), _blocked_time(0)
{
}

Client::Client(std::string const &nickname, std::string const &token, int fd):
	nickname(nickname), token(token), hopcount(0), fd(fd), _isConnected(true), _isRegistred(false),
	USER(false), NICK(true), _isInvisible(false), _isWallOps(false),
	_isServNotice(false), _isOperator(false), _restricted(false), _away(false),
	_away_message(""), _blocked(false), _blocked_time(0)
{
}

Client::Client(std::string const &username, std::string const &realname, std::string const &token, int fd):
username(username), realname(realname), token(token), hopcount(0), fd(fd),
	_isConnected(true), _isRegistred(false), USER(true), NICK(false),
	_isInvisible(false), _isWallOps(false), _isServNotice(false), _isOperator(false),
	_restricted(false), _away(false), _away_message(""), _blocked(false), _blocked_time(0)
{
}

Client::Client(const std::vector<std::string> &split, int fd) : nickname(split[1]),
	username(split[3]), realname(split[7]), hostname(split[4]), token(split[5]),
	hopcount(STOI(split[2])), fd(fd), _isConnected(true), _isRegistred(true),
	USER(true), NICK(true), _isInvisible(false), _isWallOps(false),
	_isServNotice(false), _isOperator(false), _restricted(false), _away(false), _away_message(""), _blocked(false), _blocked_time(0) {} 

Client::~Client()
{
}

Client::Client(Client const &other)
{
	*this = other;
}

Client				&Client::operator=(Client const &other)
{
	this->nickname = other.nickname;
	this->username = other.username;
	this->realname = other.realname;
	this->hostname = other.hostname;
	this->token = other.token;
	this->hopcount = other.hopcount;
	this->fd = other.fd;
	this->_isConnected = other._isConnected;
	this->_isRegistred = other._isRegistred;
	this->USER = other.USER;
	this->NICK = other.NICK;
	this->_isInvisible = other._isInvisible;
	this->_isWallOps = other._isWallOps;
	this->_isServNotice = other._isServNotice;
	this->_isOperator = other._isOperator;
	this->_restricted = other._restricted;
	this->_away = other._away;
	this->_away_message = other._away_message;
	this->_blocked = other._blocked;
	this->_blocked_time = other._blocked_time;
	this->_blocked_server = other._blocked_server;
	this->dtloggedin = other.dtloggedin;
	this->dtlastactive = other.dtlastactive;
	this->_channels = other._channels;
	this->invited = other.invited;
	return (*this);
}

bool				Client::isConnected(void)
{
	return (this->_isConnected);
}

int const 			&Client::getFD(void)
{
	return (this->fd);
}

std::string const	&Client::getnick(void)
{
	return (this->nickname);
}

std::string const	&Client::getusername(void)
{
	return (this->username);
}

std::string const	&Client::getrealname(void)
{
	return (this->realname);
}

std::string const	&Client::gethostname(void)
{
	return (this->hostname);
}

Client	*Client::getptr(void)
{
	Client	*ptr = this;
	return ptr;
}

std::string			Client::gethopcount(bool str, bool plus)
{
	std::string	hop;

	if (str)
		hop += " ";
	hop += TOSTRING(this->hopcount + (plus ? 1 : 0));
	if (str)
		hop += " ";
	return hop;
}

int					Client::gethop(void)
{
	return this->hopcount;
}

std::string	const	&Client::gettoken(void)
{
	return (this->token);
}

void	Client::sethostname(std::string const &host)
{
	this->hostname = host;
}

bool	Client::Register(std::string const &user, std::string const &real)
{
	if (this->_isRegistred ||
		user.length() < 1 || user.length() > 10 ||
		real.length() < 1)
		return (false);
	this->username = user;
	this->realname = real;
	if (NICK)
		this->_isRegistred = true;
	this->USER = true;
	this->dtloggedin = ft_getcurrenttime();
	this->dtlastactive = this->dtloggedin;
	if (DEBUG_MODE)
		std::cout << "fd " << this->fd << " registred as " <<
			this->nickname << " from " << this->username << "@" <<
			this->hostname << " (" << this->realname << ") " <<
			ft_timetostring(this->dtloggedin) << std::endl;
	return (true);
}

bool	Client::Register(std::string const &nick)
{
	this->nickname = nick;
	if (this->USER)
		_isRegistred = true;
	this->NICK = true;
	this->dtloggedin = ft_getcurrenttime();
	this->dtlastactive = this->dtloggedin;
	if (DEBUG_MODE)
		std::cout << "fd " << this->fd << " registred as " <<
			this->nickname << " from " << this->username << "@" <<
			this->hostname << " (" << this->realname << ") " <<
			ft_timetostring(this->dtloggedin) << std::endl;
	return (true);
}

time_t	Client::gettimeloggedin(void)
{
	return (this->dtloggedin);
}

time_t	Client::gettimelastactive(void)
{
	return (this->dtlastactive);
}

void	Client::updatelastactive(void)
{
	this->dtlastactive = ft_getcurrenttime();
}

void	Client::ChangeNick(std::string const &what)
{
	this->nickname = what;
}

bool	Client::isRegistred(void)
{
	return (this->_isRegistred);
}

void	Client::setFD(int what)
{
	this->fd = what;
}

bool	Client::getUSER(void)
{
	return (this->USER);
}

bool	Client::getNICK(void)
{
	return (this->NICK);
}

void	Client::setOPER(void)
{
	this->_isOperator = true;
}

bool	Client::isOperator(void)
{
	return (this->_isOperator);
}

bool	Client::isWallOps(void)
{
	return _isWallOps;
}

bool	Client::setMode(std::string const &modes)
{
	size_t	i;
	bool	set_how;

	if (modes.find_first_not_of("+-wois") != std::string::npos)
		return true;
	if (modes[0] == '+')
		set_how = true;
	else if (modes[0] == '-')
		set_how = false;
	else
		return true;
	i = 0;
	while (++i < modes.size())
	{
		if (modes[i] == 'o')
			_isOperator = set_how;
		else if (modes[i] == 'i')
			_isInvisible = set_how;
		else if (modes[i] == 'w')
			_isWallOps = set_how;
		else if (modes[i] == 's')
			_isServNotice = set_how;
	}
	return false;
}

bool	Client::setUMODE(std::string const &mode)
{
	size_t	i;
	bool	set;

	if (mode[0] == '+')
		set = true;
	else
		set = false;
	i = 0;
	while (++i < mode.size())
	{
		if (mode[i] == 'o' && set == false)
			_isOperator = set;
		else if (mode[i] == 'i')
			_isInvisible = set;
		else if (mode[i] == 'w')
			_isWallOps = set;
		else if (mode[i] == 'r' && set == true)
			_restricted = set;
		else if (mode[i] == 'O' && set == false)
			_isOperator = set;
		else if (mode[i] == 's')
			_isServNotice = set;
		else if (mode[i] == 'a')
		{
			_away = set;
			if (!set)
				_away_message.erase();
		}
	}
	return 0;
}

bool	Client::isRestricted(void)
{
	return _restricted;
}

void	Client::setAway(std::string const &msg)
{
	_away_message = msg;
	_away = true;
}

std::string	Client::getMode(bool str)
{
	std::string	mode;

	if (str)
		mode += " ";
	mode += "+";
	if (_isOperator)
		mode += "o";
	if (_isInvisible)
		mode += "i";
	if (_isWallOps)
		mode += "w";
	if (_isServNotice)
		mode += "s";
	if (_away)
		mode += "a";
	if (str)
		mode += " ";
	return (mode);
}

std::list<Channel*> &Client::getchannels(void)
{
	return _channels;
}

void	Client::add_channel(Channel *chan)
{
	_channels.push_back(chan);
}

void	Client::invite_to(const std::string &chan)
{
	invited.push_back(chan);
}

bool	Client::isInvisible(void)
{
	return (this->_isInvisible);
}

bool	Client::isInvited(Channel *chan)
{
	std::list<std::string>::iterator	begin;
	std::list<std::string>::const_iterator	mask;

	for (begin = invited.begin(); begin != invited.end(); begin++)
		if (*begin == chan->getname())
			return true;
	mask = chan->getflags()._Invitation_mask.begin();
	for (; mask != chan->getflags()._Invitation_mask.end(); mask++)
		if (match(this->getinfo(), *mask))
			return true;
	return false;
}

bool	Client::isBanned(Channel *chan)
{
	std::list<std::string>::const_iterator	ban_mask;
	std::list<std::string>::const_iterator	exception_mask;

	exception_mask = chan->getflags()._exception_mask.begin();
	for (; exception_mask != chan->getflags()._exception_mask.end(); exception_mask++)
		if (match(this->getinfo(), *exception_mask))
			return false;
	ban_mask = chan->getflags()._ban_mask.begin();
	for (; ban_mask != chan->getflags()._ban_mask.end(); ban_mask++)
	{
		if (match(this->getinfo(), *ban_mask))
			return true;
	}
	return false;
}

void	Client::block(void)
{
	_blocked = true;
	_blocked_time = ft_getcurrenttime();
}

void	Client::unblock(void)
{
	_blocked = false;
}

bool	Client::isBlocked(void)
{
	return _blocked;
}

time_t	Client::getBlockedTime(void)
{
	return _blocked_time;
}

std::string	Client::getinfo(void)
{
	return (std::string(nickname + "!" + username + "@" + hostname));
}

bool	Client::isAway(void)
{
	return (this->_away);
}

std::string const	&Client::getAwayMsg(void)
{
	return (this->_away_message);
}

std::string	Client::getsafeinfo(void)
{
	return (std::string(nickname + "!" + username + "@hiddenhost"));
}

Channel	*Client::eraseChannel(std::string const &name)
{
	std::list<Channel*>::iterator	chan;

	for (chan = _channels.begin(); chan != _channels.end(); chan++)
		if ((*chan)->getname() == name)
		{
			Channel	*res = (*chan);
			(*chan)->getclients().erase(this);
			_channels.erase(chan);
			return res;
		}
	return 0;
}

void	Client::partAllChan(void)
{
	std::list<Channel*>::iterator	chan;
	for (chan = _channels.begin(); chan != _channels.end(); chan++)
		(*chan)->getclients().erase(this);
	_channels.clear();
}
