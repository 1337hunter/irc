/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/10 17:24:05 by salec             #+#    #+#             */
/*   Updated: 2020/12/19 21:53:29 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <vector>
// fcntl, select, socket, inet structs, inet
#include <fcntl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
// read, write
#include <unistd.h>
#include "common_defines.hpp"
#include "channel.hpp"
#include <list>

/*	A client is anything connecting to a server that is not another
	server.  Each client is distinguished from other clients by a unique
	nickname having a maximum length of nine (9) characters.
	See the protocol grammar rules for what may and may not be used in a
	nickname.  In addition to the nickname, all servers must have the
	following information about all clients: the real name of the host
	that the client is running on, the username of the client on that
	host, and the server to which the client is connected.	*/

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
	bool			_isInvisible; // User modes
	bool			_isWallOps;
	bool			_isServNotice;
	bool			_isOperator; // User modes
	bool			_restricted;
	bool			_away;
	std::string		_away_message;
	bool			_blocked;
	time_t			dtloggedin;
	std::list<Channel*>	_channels; //unordered map is better channame->Channel*
	std::list<std::string>	invited;
	// Maybe server class object reference or pointer here
	Client(void);
public:
	Client(std::string const &nickname, std::string const &token, int fd);
	Client(std::string const &username, std::string const &realname,
		   std::string const &token, int fd);
	Client(std::string const &nick, std::string const hop, std::string const &user,
		std::string const &host, std::string const &servertoken,
		std::string const umode, std::string const &real);
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
	std::string			gethopcount(bool str = false, bool plus = false);
	int					gethop(void);
	std::list<Channel*>	&getchannels(void);
	Channel				*eraseChannel(std::string const &name);
	void				add_channel(Channel *chan);
	void				invite_to(const std::string &chan);
	bool				isInvited(Channel *chan);
	bool				isBlocked(void);
	bool				isRestricted(void);
	void				block(void);
	void				unblock(void);
	bool				isBanned(Channel *chan);
	void				sethostname(std::string const &host);

	bool				Register(std::string const &user,
							std::string const &real);
	bool				Register(std::string const &nick);
	void				ChangeNick(std::string const &what);
	bool				setMode(std::string const &modes);
	bool				setUMODE(std::string const &mode);
	void				setAway(std::string const &msg);
	std::string			getMode(bool str = false);
//	void				Reconnect(int fd);
//	void				Disconnect(void);
};

#endif
