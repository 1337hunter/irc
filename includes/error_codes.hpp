/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_codes.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/21 20:58:56 by salec             #+#    #+#             */
/*   Updated: 2020/10/22 14:44:17 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_CODES_HPP
#define ERROR_CODES_HPP

//	"<nickname> :No such nick/channel"
#define ERR_NOSUCHNICK			401
//	"<server name> :No such server"
#define ERR_NOSUCHSERVER		402
//	"<channel name> :No such channel"
#define ERR_NOSUCHCHANNEL		403
//	"<channel name> :Cannot send to channel"
#define ERR_CANNOTSENDTOCHAN	404
//	"<channel name> :You have joined too many channels"
#define ERR_TOOMANYCHANNELS	405
//	"<nickname> :There was no such nickname"
#define ERR_WASNOSUCHNICK		406
//	"<target> :Duplicate recipients. No message delivered"
#define ERR_TOOMANYTARGETS		407
//	":No origin specified"
#define ERR_NOORIGIN			409
//	":No recipient given (<command>)"
#define ERR_NORECIPIENT		411
//	":No text to send"
#define ERR_NOTEXTTOSEND		412
//	"<mask> :No toplevel domain specified"
#define ERR_NOTOPLEVEL			413
//	"<mask> :Wildcard in toplevel domain"
#define ERR_WILDTOPLEVEL		414
//	"<command> :Unknown command"
#define ERR_UNKNOWNCOMMAND		421
//	":MOTD File is missing"
#define ERR_NOMOTD				422
//	"<server> :No administrative info available"
#define ERR_NOADMININFO		423
//	":File error doing <file op> on <file>"
#define ERR_FILEERROR			424
//	":No nickname given"
#define ERR_NONICKNAMEGIVEN	431
//	"<nick> :Erroneus nickname"
#define ERR_ERRONEUSNICKNAME	432
//	"<nick> :Nickname is already in use"
#define ERR_NICKNAMEINUSE		433
//	"<nick> :Nickname collision KILL"
#define ERR_NICKCOLLISION		436
//	"<nick> <channel> :They aren't on that channel"
#define ERR_USERNOTINCHANNEL	441
//	"<channel> :You're not on that channel"
#define ERR_NOTONCHANNEL		442
//	"<user> <channel> :is already on channel"
#define ERR_USERONCHANNEL		443
//	"<user> :User not logged in"
#define ERR_NOLOGIN			444
//	":SUMMON has been disabled"
#define ERR_SUMMONDISABLED		445
//	":USERS has been disabled"
#define ERR_USERSDISABLED		446
//	":You have not registered"
#define ERR_NOTREGISTERED		451
//	"<command> :Not enough parameters"
#define ERR_NEEDMOREPARAMS		461
//	":You may not reregister"
#define ERR_ALREADYREGISTRED	462
//	":Your host isn't among the privileged"
#define ERR_NOPERMFORHOST		463
//	":Password incorrect"
#define ERR_PASSWDMISMATCH		464
//	":You are banned from this server"
#define ERR_YOUREBANNEDCREEP	465
//	"<channel> :Channel key already set"
#define ERR_KEYSET				467
//	"<channel> :Cannot join channel (+l)"
#define ERR_CHANNELISFULL		471
//	"<char> :is unknown mode char to me"
#define ERR_UNKNOWNMODE		472
//	"<channel> :Cannot join channel (+i)"
#define ERR_INVITEONLYCHAN		473
//	"<channel> :Cannot join channel (+b)"
#define ERR_BANNEDFROMCHAN		474
//	"<channel> :Cannot join channel (+k)"
#define ERR_BADCHANNELKEY		475
//	":Permission Denied- You're not an IRC operator"
#define ERR_NOPRIVILEGES		481
//	"<channel> :You're not channel operator"
#define ERR_CHANOPRIVSNEEDED	482
//	":You cant kill a server!"
#define ERR_CANTKILLSERVER		483
//	":No O-lines for your host"
#define ERR_NOOPERHOST			491
//	":Unknown MODE flag"
#define ERR_UMODEUNKNOWNFLAG	501
//	":Cant change mode for other users"
#define ERR_USERSDONTMATCH		502

#endif
