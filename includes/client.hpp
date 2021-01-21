#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>
# include <vector>
# include <fcntl.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>
# include "common_defines.hpp"
# include "channel.hpp"
# include <list>

class Channel;

class Client {
private:
	std::string		nickname;
	std::string		username;
	std::string		realname;
	std::string		hostname;
	std::string		token;
	int				hopcount;
	int				fd;
	bool			_isConnected;
	bool			_isRegistred;
	bool			USER;
	bool			NICK;
	bool			_isInvisible;
	bool			_isWallOps;
	bool			_isServNotice;
	bool			_isOperator;
	bool			_restricted;
	bool			_away;
	std::string		_away_message;
	bool			_blocked;
	time_t			_blocked_time;
	std::string		_blocked_server;
	time_t			dtloggedin;
	time_t			dtlastactive;
	std::list<Channel*>	_channels;
	std::list<std::string>	invited;
	Client(void);
public:
	Client(std::string const &nickname, std::string const &token, int fd);
	Client(std::string const &username, std::string const &realname,
		   std::string const &token, int fd);
	Client(const std::vector<std::string> &split, int _fd);
	Client(int fd, std::string const &username);
	~Client();
	Client(Client const &other);
	Client &operator=(Client const &other);

	// getters-setters here
	bool				isConnected(void);
	bool				isRegistred(void);
	bool				isOperator(void);
	bool				isInvisible(void);
	bool				isWallOps(void);
	bool				isAway(void);
	bool				getUSER(void);
	bool				getNICK(void);
	int const	 		&getFD(void);
	void				setFD(int fd);
	void				setOPER(void);
	std::string const	&getnick(void);
	std::string const	&getusername(void);
	std::string const	&getrealname(void);
	std::string const	&gethostname(void);
	std::string	const	&gettoken(void);
	std::string			getinfo(void);
	std::string			getsafeinfo(void);
	Client				*getptr(void);
	time_t				gettimeloggedin(void);
	time_t				gettimelastactive(void);
	void				updatelastactive(void);
	std::string			gethopcount(bool str = false, bool plus = false);
	int					gethop(void);
	std::list<Channel*>	&getchannels(void);
	Channel				*eraseChannel(std::string const &name);
	void				add_channel(Channel *chan);
	void				invite_to(const std::string &chan);
	bool				isInvited(Channel *chan);
	bool				isBlocked(void);
	time_t				getBlockedTime(void);
	bool				isRestricted(void);
	void				block(void);
	void				unblock(void);
	bool				isBanned(Channel *chan);
	void				sethostname(std::string const &host);
	void				partAllChan(void);

	bool				Register(std::string const &user,
							std::string const &real);
	bool				Register(std::string const &nick);
	void				ChangeNick(std::string const &what);
	bool				setMode(std::string const &modes);
	bool				setUMODE(std::string const &mode);
	void				setAway(std::string const &msg);
	std::string const	&getAwayMsg(void);
	std::string			getMode(bool str = false);
};

#endif
