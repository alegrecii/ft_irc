#pragma once

#include "utils.hpp"

#define SUPERCHANNEL "#welcome"

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
	unsigned long int					_fdCount;
	bool const							_isPassword;

	std::list<Client*>					_clientsNotRegistered;
	std::map<std::string, Client*>		_clients;
	std::map<std::string, Channel*>		_channels;
	std::map<std::string, commandFunct>	_commands;

public:
	Server(const std::string &port, const std::string &psw);
	~Server();

	const std::string	&getPassword() const;

	void				updateNick(Client &client, const std::string &newName);
	Client				*getClient(const std::string &name);
	Channel				*getChannel(const std::string &chName);
	Client				*getClientByFd(int fd) const;
	void				addChannel(Channel *ch);
	void				deleteClient(Client *client);
	void				deleteClient(const std::string &clName);
	void				deleteChannel(Channel *channel);
	void				deleteChannel(const std::string &chName);

	void				run();
	void				status();

	void				welcomeMessage(Client &client);
	void				msgAnalyzer(Client &client, const char *msg);
	void				cmdAnalyzer(Client &client, std::vector<std::string> vParam);
	void				registration(Client &client, std::vector<std::string> vParam);
	void				sendToAllClients(const std::string &msg);
	std::string			findUsers(const std::string &name);
};

