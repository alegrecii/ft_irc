#include "Command.hpp"

void	Command::join(Server &server, Client &client, std::vector<std::string> &v)
{
	(void)server;
	(void)client;
	(void)v;

	std::cout << "Command detected: JOIN" << std::endl;
	std::cout << "Parameters: " << std::endl;
	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it)
	{
		std::cout << *it << std::endl;
	}
}

void	Command::privmsg(Server &server, Client &client, std::vector<std::string> &v)
{
	(void)server;
	(void)client;
	(void)v;

	std::cout << "Command detected: PRIVMSG" << std::endl;
	std::cout << "Parameters: " << std::endl;
	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it)
	{
		std::cout << *it << std::endl;
	}
}

void	Command::pong(Server &server, Client &client, std::vector<std::string> &v)
{
	(void)server;
	(void)client;
	(void)v;

	std::cout << "Command detected: PONG" << std::endl;
	std::cout << "Parameters: " << std::endl;
	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it)
	{
		std::cout << *it << std::endl;
	}
}

void	Command::ping(Server &server, Client &client, std::vector<std::string> &v)
{
	(void)server;
	(void)client;
	(void)v;
}

void	Command::kick(Server &server, Client &client, std::vector<std::string> &v)
{
	(void)server;
	(void)client;
	(void)v;
}

void	Command::invite(Server &server, Client &client, std::vector<std::string> &v)
{
	(void)server;
	(void)client;
	(void)v;
}

void	Command::topic(Server &server, Client &client, std::vector<std::string> &v)
{
	(void)server;
	(void)client;
	(void)v;	
}

void	Command::mode(Server &server, Client &client, std::vector<std::string> &v)
{
	(void)server;
	(void)client;
	(void)v;
}

