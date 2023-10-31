#pragma once

#include "utils.hpp"

class Server;
class Client;
class Command;
class Channel;

typedef	void	(*commandFunct)(Server &server, Client &client, std::vector<std::string> &v);

class Server
{
private:
	uint16_t const						_port;
	std::string const					_psw;
	bool const							_isPassword;
	std::map<int, Client>				_clients;
	std::map<std::string, commandFunct>	_commands;

	std::map<std::string, Channel>		_channels;
public:
	Server(const std::string &port, const std::string &psw);
	~Server();


	void	run();
	void	msgAnalyzer(Client &client, const char *msg);
	void	registration(Client &client, const std::string &msg);
	void	cmdAnalyzer(Client &client, const std::string &msg);
	void	welcomeMessage(Client &client);
	void	setChannels(const std::string &name, const std::string &pass, const std::string &nameClient);
};

