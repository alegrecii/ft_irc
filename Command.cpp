#include "Command.hpp"

void	Command::join(Server &server, Client &client, std::vector<std::string> &v)
{
	std::cout << "Command detected: JOIN" << std::endl;
	if (v.size() < 1)
	{
		std::string error = ":" + client.getNickname() + " 461 :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}
	std::string	name, pass;
	std::istringstream param1(v[0]);
	if (v.size() == 2)
	{
		std::istringstream param2(v[1]);
		while(std::getline(param1, name, ','))
		{
			if (name[0] == '#' || name[0] == '&')
			{
				if(!std::getline(param2, pass, ','))
					pass = "";
				server.setChannels(name, pass, client);
				//RPL_JOIN , RPL_TOPIC, RPL_NAMREPLY, RPL_ENDOFNAMES
			}
		}
	}
	else if (v.size() == 1)
	{
		while(std::getline(param1, name, ','))
		{
			if (name[0] == '#' || name[0] == '&')
			{
				pass = "";
				server.setChannels(name, pass, client);
				//RPL_JOIN , RPL_TOPIC, RPL_NAMREPLY, RPL_ENDOFNAMES
			}
		}
	}
}

void	Command::privmsg(Server &server, Client &client, std::vector<std::string> &v)
{
	std::cout << "Command detected: PRIVMSG" << std::endl;
	(void) server;
	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it)
	{
		std::cout << *it << std::endl;
	}
	if (v.size() < 2)
	{
		std::string error = "461 " + client.getNickname() + "PRIVMSG :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}
}

void	Command::ping(Server &server, Client &client, std::vector<std::string> &v)
{
	(void)	server;
	(void)	client;
	if (!v.empty())
	{
		std::string token = v[0];
		std::string pong = "PONG server " + token + "\n";
		send(client.getFd(), pong.c_str(), pong.length(), 0);
	}
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

