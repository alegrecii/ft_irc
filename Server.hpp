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

	std::list<Client*>					_clientsNotRegistered;
	std::map<std::string, Client*>		_clients;
	std::map<std::string, Channel*>		_channels;
	std::map<std::string, commandFunct>	_commands;

public:
	Server(const std::string &port, const std::string &psw);
	~Server();

	const std::string	&getPassword() const;

	void	updateNick(Client &client, const std::string &newName);
	Client	*getClient(const std::string &name);
	Channel	*getChannel(const std::string &chName);
	Client	*getClientByFd(int fd) const;
	void	deleteClientByFd(int fd);

	void	run();
	void	msgAnalyzer(Client &client, const char *msg);
	void	registration(Client &client, const std::string &msg);
	void	cmdAnalyzer(Client &client, const std::string &msg);
	void	welcomeMessage(Client &client);
	void	setChannels(const std::string &name, const std::string &pass, Client &client);
	void	sendJoin(const std::string &name, Client &client);
};

