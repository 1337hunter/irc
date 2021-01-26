#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>
# include <vector>
# include "client.hpp"
# include <list>
# ifndef STD_CPP98
#  include <unordered_map>
# else
#  include <map>
#endif

class Client;
typedef std::vector<std::string>	t_strvect;

struct	channel_flags
{
	bool		_anonymous;
	bool		_invite_only;
	bool		_moderated;
	bool		_no_messages_outside;
	bool		_quiet;
	bool		_private;
	bool		_secret;
	bool		_reop;
	bool		_topic_settable_by_chop;
	std::string	_key;
	size_t		_limit_of_users;
	std::list<std::string>	_ban_mask;
	std::list<std::string>	_exception_mask;
	std::list<std::string>	_Invitation_mask;

	channel_flags(const channel_flags &o);
	channel_flags &operator=(const channel_flags &o);
	channel_flags(void);
	~channel_flags(void);
};

struct	client_flags
{
	bool	_Operator;
	bool	_operator;
	bool	_voice;

	client_flags(void);
	client_flags(bool Operator, bool oper, bool voice);
	client_flags(client_flags const &obj);
	client_flags(bool *mod);
	~client_flags(void) {}
	client_flags &operator=(client_flags const &obj);
};

class Channel {
private:
	std::string				_name;
	bool                    _blocked;
	time_t					_blocked_time;
	time_t					_creation_time;
	int						_reop_delay;
	std::string				_safe_postfix;
	std::string				_topic;
	char					_type;
	channel_flags			_flags;
	std::MAP<Client*, client_flags>	_clients;
	std::string				_channel_creator;
	Channel();
public:
	Channel(std::string const &name);
	Channel(std::string const &name, Client *client);
	Channel(std::string const &name, Client *client, std::string const &modes);
	Channel(std::string const &name, std::string const &key, Client *client);
	~Channel();
	Channel(Channel const &other);
	Channel &operator=(Channel const &other);

	std::MAP<Client*, client_flags> &getclients(void);
	std::string const	&getname(void);
	std::string	const	&getkey(void);
	std::string const	&gettopic(void);
	std::string const	&getCreator(void);
	std::string			getCreationTime(void);
	std::string			getMode(void);
	std::string			size(void);
	channel_flags const	&getflags(void);
	Channel				*getptr(void);
	char				gettype(void);

	void				add_client(Client *clint, bool O, bool o, bool v);
	void				add_client(Client *client);
	void				block(void);
	void				unblock(void);

	bool				isBlocked(void);
	time_t				getBlockedTime(void);
	bool				block_if(void);
	bool				isOnChan(Client *client);
	bool				isInvited(Client *client);
	bool				isBanned(Client *client);
	bool				isOperator(Client *client); // not safe (use isOnChan before);
	bool				isSecret(void);
	bool				isPrivate(void);

	int					setMode(std::string const &mode);
	int					setMode(t_strvect const &args);
	void				settopic(std::string const &topic);
};

#endif
