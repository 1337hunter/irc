/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_codes.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/21 20:58:56 by salec             #+#    #+#             */
/*   Updated: 2020/10/26 13:25:12 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_CODES_HPP
#define ERROR_CODES_HPP

//	"<nickname> :No such nick/channel"
#define ERR_NOSUCHNICK			401
/*	- Used to indicate the nickname parameter supplied to a
	command is currently unused.	*/

//	"<server name> :No such server"
#define ERR_NOSUCHSERVER		402
/*	- Used to indicate the server name given currently does not exist.	*/

//	"<channel name> :No such channel"
#define ERR_NOSUCHCHANNEL		403
/*	- Used to indicate the given channel name is invalid.	*/

//	"<channel name> :Cannot send to channel"
#define ERR_CANNOTSENDTOCHAN	404
/*	- Sent to a user who is either (a) not on a channel
	which is mode +n or (b) not a chanop (or mode +v) on
	a channel which has mode +m set or where the user is
	banned and is trying to send a PRIVMSG message to
	that channel.	*/

//	"<channel name> :You have joined too many channels"
#define ERR_TOOMANYCHANNELS		405
/*	- Sent to a user when they have joined the maximum
	number of allowed channels and they try to join another channel.	*/

//	"<nickname> :There was no such nickname"
#define ERR_WASNOSUCHNICK		406
/*	- Returned by WHOWAS to indicate there is no history
	information for that nickname.	*/

/*	"<target> :Duplicate recipients. No message delivered"
	or
	"<target> :<error code> recipients. <abort message>"	*/
#define ERR_TOOMANYTARGETS		407
/*	- Returned to a client which is attempting to send a
	PRIVMSG/NOTICE using the user@host destination format
	and for a user@host which has several occurrences.
	- Returned to a client which trying to send a
	PRIVMSG/NOTICE to too many recipients.
	- Returned to a client which is attempting to JOIN a safe
	channel using the shortname when there are more than one such channel.	*/

//	"<service name> :No such service"
#define ERR_NOSUCHSERVICE		408
/*	- Returned to a client which is attempting to send a SQUERY
	to a service which does not exist.	*/

//	":No origin specified"
#define ERR_NOORIGIN			409
/*	- PING or PONG message missing the originator parameter.	*/

//	":No recipient given (<command>)"
#define ERR_NORECIPIENT			411
//	":No text to send"
#define ERR_NOTEXTTOSEND		412
//	"<mask> :No toplevel domain specified"
#define ERR_NOTOPLEVEL			413
//	"<mask> :Wildcard in toplevel domain"
#define ERR_WILDTOPLEVEL		414
//	"<mask> :Bad Server/host mask"
#define ERR_BADMASK				415
/*	- 412 - 415 are returned by PRIVMSG to indicate that
	the message wasn't delivered for some reason.
	ERR_NOTOPLEVEL and ERR_WILDTOPLEVEL are errors that
	are returned when an invalid use of
	"PRIVMSG $<server>" or "PRIVMSG #<host>" is attempted.	*/

//	"<command> :Unknown command"
#define ERR_UNKNOWNCOMMAND		421
/*	- Returned to a registered client to indicate that the
	command sent is unknown by the server.	*/

//	":MOTD File is missing"
#define ERR_NOMOTD				422
/*	- Server's MOTD file could not be opened by the server.	*/

//	"<server> :No administrative info available"
#define ERR_NOADMININFO			423
/*	- Returned by a server in response to an ADMIN message
	when there is an error in finding the appropriate information.	*/

//	":File error doing <file op> on <file>"
#define ERR_FILEERROR			424
/*	- Generic error message used to report a failed file
	operation during the processing of a message.	*/

//	":No nickname given"
#define ERR_NONICKNAMEGIVEN		431
/*	- Returned when a nickname parameter expected for a
	command and isn't found.	*/

//	"<nick> :Erroneus nickname"
#define ERR_ERRONEUSNICKNAME	432
/*	- Returned after receiving a NICK message which contains
	characters which do not fall in the defined set.  See
	section 2.3.1 for details on valid nicknames.	*/

//	"<nick> :Nickname is already in use"
#define ERR_NICKNAMEINUSE		433
/*	- Returned when a NICK message is processed that results
	in an attempt to change to a currently existing nickname.	*/

/*	"<nick> :Nickname collision KILL"
	or
	"<nick> :Nickname collision KILL from <user>@<host>"	*/
#define ERR_NICKCOLLISION		436
/*	- Returned by a server to a client when it detects a
	nickname collision (registered of a NICK that
	already exists by another server).	*/

//	"<nick/channel> :Nick/channel is temporarily unavailable"
#define ERR_UNAVAILRESOURCE		437
/*	- Returned by a server to a user trying to join a channel
	currently blocked by the channel delay mechanism.
	- Returned by a server to a user trying to change nickname
	when the desired nickname is blocked by the nick delay mechanism.	*/

//	"<nick> <channel> :They aren't on that channel"
#define ERR_USERNOTINCHANNEL	441
/*	- Returned by the server to indicate that the target
	user of the command is not on the given channel.	*/

//	"<channel> :You're not on that channel"
#define ERR_NOTONCHANNEL		442
/*	- Returned by the server whenever a client tries to	perform
	a channel affecting command for which the client isn't a member.	*/

//	"<user> <channel> :is already on channel"
#define ERR_USERONCHANNEL		443
/*	- Returned when a client tries to invite a user to a
	channel they are already on.	*/

//	"<user> :User not logged in"
#define ERR_NOLOGIN				444
/*	- Returned by the summon after a SUMMON command for a
	user was unable to be performed since they were not logged in.	*/

//	":SUMMON has been disabled"
#define ERR_SUMMONDISABLED		445
/*	- Returned as a response to the SUMMON command.  MUST be
	returned by any server which doesn't implement it.	*/

//	":USERS has been disabled"
#define ERR_USERSDISABLED		446
/*	- Returned as a response to the USERS command.  MUST be
	returned by any server which does not implement it.	*/

//	":You have not registered"
#define ERR_NOTREGISTERED		451
/*	- Returned by the server to indicate that the client
	MUST be registered before the server will allow it
	to be parsed in detail.		*/

//	"<command> :Not enough parameters"
#define ERR_NEEDMOREPARAMS		461
/*	- Returned by the server by numerous commands to
	indicate to the client that it didn't supply enough parameters.	*/

/*	":You may not reregister"
	or
	":Unauthorized command (already registered)"	*/
#define ERR_ALREADYREGISTRED	462
/*	- Returned by the server to any link which tries to
	change part of the registered details (such as
	password or user details from second USER message).	*/

//	":Your host isn't among the privileged"
#define ERR_NOPERMFORHOST		463
/*	- Returned to a client which attempts to register with
	a server which does not been setup to allow
	connections from the host the attempted connection
	is tried.	*/

//	":Password incorrect"
#define ERR_PASSWDMISMATCH		464
/*	- Returned to indicate a failed attempt at registering
	a connection for which a password was required and
	was either not given or incorrect.	*/

//	":You are banned from this server"
#define ERR_YOUREBANNEDCREEP	465
/*	- Returned after an attempt to connect and register
	yourself with a server which has been setup to
	explicitly deny connections to you.	*/

#define ERR_YOUWILLBEBANNED		466
/*	- Sent by a server to a user to inform that access to the
	server will soon be denied.		*/

//	"<channel> :Channel key already set"
#define ERR_KEYSET				467
//	"<channel> :Cannot join channel (+l)"
#define ERR_CHANNELISFULL		471
/*	"<char> :is unknown mode char to me"
	or
	"<char> :is unknown mode char to me for <channel>"	*/
#define ERR_UNKNOWNMODE			472
//	"<channel> :Cannot join channel (+i)"
#define ERR_INVITEONLYCHAN		473
//	"<channel> :Cannot join channel (+b)"
#define ERR_BANNEDFROMCHAN		474
//	"<channel> :Cannot join channel (+k)"
#define ERR_BADCHANNELKEY		475
//	"<channel> :Bad Channel Mask"
#define ERR_BADCHANMASK			476
// "<channel> :Channel doesn't support modes"
#define ERR_NOCHANMODES			477
// "<channel> <char> :Channel list is full"
#define ERR_BANLISTFULL			478

//	":Permission Denied- You're not an IRC operator"
#define ERR_NOPRIVILEGES		481
/*	- Any command requiring operator privileges to operate
	MUST return this error to indicate the attempt was
	unsuccessful.	*/

//	"<channel> :You're not channel operator"
#define ERR_CHANOPRIVSNEEDED	482
/*	- Any command requiring 'chanop' privileges (such as
	MODE messages) MUST return this error if the client
	making the attempt is not a chanop on the specified
	channel.	*/

//	":You cant kill a server!"
#define ERR_CANTKILLSERVER		483
/*	- Any attempts to use the KILL command on a server
	are to be refused and this error returned directly
	to the client.	*/

//	":Your connection is restricted!"
#define ERR_RESTRICTED			484
/*	- Sent by the server to a user upon connection to indicate
	the restricted nature of the connection (user mode "+r").	*/

//	":You're not the original channel operator"
#define ERR_UNIQOPPRIVSNEEDED	485
/*	- Any MODE requiring "channel creator" privileges MUST
	return this error if the client making the attempt is not
	a chanop on the specified channel.	*/

//	":No O-lines for your host"
#define ERR_NOOPERHOST			491
/*	- If a client sends an OPER message and the server has
	not been configured to allow connections from the
	client's host as an operator, this error MUST be
	returned.	*/

//	":Unknown MODE flag"
#define ERR_UMODEUNKNOWNFLAG	501
/*	- Returned by the server to indicate that a MODE
	message was sent with a nickname parameter and that
	the a mode flag sent was not recognized.	*/

/*	":Cant change mode for other users"
	or
	":Cannot change mode for other users"	*/
#define ERR_USERSDONTMATCH		502
/*	- Error sent to any user trying to view or change the
	user mode for a user other than themselves.	*/

//	reserved
#define ERR_NOSERVICEHOST		492

#endif
