#pragma once

#include "utils.hpp"

class Server;

class Client;

/*
	-USER
	-NICK
	-PASS

	-JOIN
	-PRIVMSG
	-PING ?
	-PONG
	
	-KICK
	-INVITE
	-TOPIC
	-MODE
*/

class Command
{
private:
	Command();
	~Command();
public:
	static void		user(Server &server, Client &client, std::vector<std::string> &v);
	static void		nick(Server &server, Client &client, std::vector<std::string> &v);
	static void		pass(Server &server, Client &client, std::vector<std::string> &v);
	static void		join(Server &server, Client &client, std::vector<std::string> &v);
	static void		privmsg(Server &server, Client &client, std::vector<std::string> &v);
	static void		ping(Server &server, Client &client, std::vector<std::string> &v);
	static void		pong(Server &server, Client &client, std::vector<std::string> &v);
	static void		kick(Server &server, Client &client, std::vector<std::string> &v);
	static void		invite(Server &server, Client &client, std::vector<std::string> &v);
	static void		topic(Server &server, Client &client, std::vector<std::string> &v);
	static void		mode(Server &server, Client &client, std::vector<std::string> &v);
};