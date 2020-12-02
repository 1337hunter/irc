/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/24 12:11:19 by salec             #+#    #+#             */
/*   Updated: 2020/12/02 20:59:23 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include "tools.hpp"

Client::Client() : fd(-1), _isConnected(false), _isRegistred(false),
	USER(false), NICK(false), _isInvisible(false), _isWallOps(false),
	_isServNotice(false), _isOperator(false)
{
}

Client::Client(std::string const &nickname, int fd):
	nickname(nickname), hopcount(0), fd(fd), _isConnected(true), _isRegistred(false),
	USER(false), NICK(true), _isInvisible(false), _isWallOps(false),
	_isServNotice(false), _isOperator(false)
{
}

Client::Client(std::string const &username, std::string const &realname, int fd):
username(username), realname(realname), hopcount(0), fd(fd), _isConnected(true),
	_isRegistred(false), USER(true), NICK(false), _isInvisible(false), _isWallOps(false),
	_isServNotice(false), _isOperator(false)
{
}

Client::Client(std::string const &nick, std::string const hop, std::string const &user, std::string const &host, std::string const &servertoken, std::string const umode, std::string const &real) :
	nickname(nick), username(user), realname(real), hostname(host),
	token(servertoken), hopcount(stoi(hop)), fd(-1), _isConnected(true),
	_isRegistred(true), USER(true), NICK(true)
{
	setMode(umode);
}

Client::Client(const std::vector<std::string> &split) : nickname(split[1]),
	username(split[3]), hostname(split[4]), token(split[5]), hopcount(stoi(split[2]))
{
	std::string	temp(split[7], 1);
	setMode(split[6]);
	realname = temp;
	for (size_t i = 8; i < split.size(); i++)
		realname += " " + split[i];
}

Client::~Client()
{
}

Client::Client(Client const &other)
{
	*this = other;
}

Client				&Client::operator=(Client const &other)
{
	this->fd = other.fd;
	this->nickname = other.nickname;
	this->username = other.username;
	this->realname = other.realname;
	this->hostname = other.hostname;
	this->_isConnected = other._isConnected;
	this->_isRegistred = other._isRegistred;
	this->_isOperator = other._isOperator;
	this->_isInvisible = other._isInvisible;
	this->_isServNotice = other._isServNotice;
	this->_isWallOps = other._isWallOps;
	this->hopcount = other.hopcount;
	this->USER = other.USER;
	this->NICK = other.NICK;
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

std::string const	&Client::getnickname(void)
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

std::string			Client::gethopcount(bool str, bool plus)
{
	std::string	hop;

	if (str)
		hop += " ";
	hop += std::to_string(this->hopcount + (plus ? 1 : 0));
	if (str)
		hop += " ";
	return hop;
}

std::string	const	&Client::gettoken(void)
{
	return this->token;
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
	this->dtloggedin = ft_getcurrenttimestring();
	if (DEBUG_MODE)
		std::cout << "fd " << this->fd << " registred as " <<
			this->nickname << " from " << this->username << "@" <<
			this->hostname << " (" << this->realname << ") " <<
			this->dtloggedin << std::endl;
	return (true);
}

bool	Client::Register(std::string const &nick)
{
	this->nickname = nick;
	if (this->USER)
		_isRegistred = true;
	this->NICK = true;
	this->dtloggedin = ft_getcurrenttimestring();
	if (DEBUG_MODE)
		std::cout << "fd " << this->fd << " registred as " <<
			this->nickname << " from " << this->username << "@" <<
			this->hostname << " (" << this->realname << ") " <<
			this->dtloggedin << std::endl;
	return (true);
}

void	Client::Reconnect(int fd)
{
	this->fd = fd;
	this->_isConnected = true;
	this->NICK = true;
}

void	Client::Disconnect(void)
{
	this->fd = -1;
	this->hostname = "";
	this->_isConnected = false;
	this->_isRegistred = false;
	this->_isOperator = false;
	this->_isInvisible = false;
	this->_isServNotice = false;
	this->_isWallOps = false;
	this->NICK = false;
	this->USER = false;
	this->token = "";
	// add to nickname history here
}

void	Client::ChangeNick(std::string const &what)
{
	this->nickname = what;
}

bool	Client::isRegistred(void)
{
	return _isRegistred;
}

void	Client::setFD(int what)
{
	fd = what;
}

bool	Client::getUSER(void)
{
	return USER;
}

bool	Client::getNICK(void)
{
	return NICK;
}

void	Client::setOPER(void)
{
	_isOperator = true;
}

bool	Client::isOperator(void)
{
	return _isOperator;
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

std::string	Client::getMode(bool str)
{
	std::string	mode;

	if (str)
		mode += " ";
	if (_isOperator || _isInvisible || _isWallOps || _isServNotice)
		mode += "+";
	else
	{
		mode += "-iwso";
		if (str)
			mode += " ";
		return mode;
	}
	if (_isOperator)
		mode += "o";
	if (_isInvisible)
		mode += "i";
	if (_isWallOps)
		mode += "w";
	if (_isServNotice)
		mode += "s";
	if (str && mode.size() > 2)
		mode += " ";
	return mode;

}

std::list<Channel*> const &Client::getchannels(void)
{
	return _channels;
}

void	Client::add_channel(Channel *chan)
{
	_channels.push_back(chan);
}
