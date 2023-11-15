#pragma once

#include "utils.hpp"

class Server;
class Channel;
class Client;

class Command
{
private:
	Command();
	~Command();

	static void			msgToChannel(Server &s, Client &c, const std::string &chName, const std::string &msg);
	static void			msgToClient(Server &s, Client &c, const std::string &targetClient, const std::string &msg);

	static std::string	findUsers(Channel &channel);
	static void			setChannels(Server &server, const std::string &name, const std::string &pass, Client &client);
	static void			sendJoin(Channel &channel, Client &client);

public:
	static void			status(Server &server, Client &client, std::vector<std::string> &v);
	static void			user(Server &server, Client &client, std::vector<std::string> &v);
	static void			pass(Server &server, Client &client, std::vector<std::string> &v);
	static void			nick(Server &server, Client &client, std::vector<std::string> &v);
	static void			join(Server &server, Client &client, std::vector<std::string> &v);
	static void			privmsg(Server &server, Client &client, std::vector<std::string> &v);
	static void			ping(Server &server, Client &client, std::vector<std::string> &v);
	static void			kick(Server &server, Client &client, std::vector<std::string> &v);
	static void			invite(Server &server, Client &client, std::vector<std::string> &v);
	static void			topic(Server &server, Client &client, std::vector<std::string> &v);
	static void			mode(Server &server, Client &client, std::vector<std::string> &v);
	static void			part(Server &server, Client &client, std::vector<std::string> &v);
	static void			who(Server &server, Client &client, std::vector<std::string> &v);
	static void			userhost(Server &server, Client &client, std::vector<std::string> &v);
	//static void			bot(Server &server, Client &client, std::vector<std::string> &v);

};
