#include "Command.hpp"

void	Command::createChannel(const std::string &name, const std::string &pass, Client &client)
{
	std::string RPL_JOIN = ":" + client.getNickname() + "!" + client.getUser() + "@localhost JOIN :" + name + "\r\n";
	std::string RPL_NAMREPLY = ":ircserv 353 " + client.getNickname() + " = " + name + " :manuel\r\n";
	std::string RPL_NAMREPLY1 = ":ircserv 353 " + client.getNickname() + " = " + name + " :@ale @damiano\r\n";
	std::string RPL_ENDOFNAMES = ":ircserv 366 " + client.getNickname() + " " + name + " :End of NAMES list\r\n";

	

	send(client.getFd(), RPL_JOIN.c_str(), RPL_JOIN.size(), 0);
	send(client.getFd(), RPL_NAMREPLY.c_str(), RPL_NAMREPLY.size(), 0);
	send(client.getFd(), RPL_NAMREPLY1.c_str(), RPL_NAMREPLY1.size(), 0);
	send(client.getFd(), RPL_ENDOFNAMES.c_str(), RPL_ENDOFNAMES.size(), 0);
	(void) pass;
	// std::string RPL_JOIN = ":" + _client_info[fd].nickname + "!" + _server.hostname + " JOIN #" + channel_name + "\r\n";
    // std::string RPL_NAMREPLY = ":ircserv 353 " + _client_info[fd].nickname + " = #" + channel_name + " :";
    // std::string RPL_ENDOFNAMES = ":ircserv 366 " + _client_info[fd].nickname + " #" + channel_name + " :End of NAMES list\r\n"
}

void	Command::join(Server &server, Client &client, std::vector<std::string> &v)
{
	std::cout << "Command detected: JOIN" << std::endl;
	if (v.size() < 1)
	{
		std::string error = client.getNickname() + " 461 :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}
	std::string	name, pass;
	std::istringstream param1(v[0]), param2(v[1]);
	while(std::getline(param1, name, ','))
	{
		if (name[0] == '#' || name[0] == '&')
		{
			if(!std::getline(param2, pass, ','))
				pass = "";
			server.setChannels(name, pass, client.getNickname());
			createChannel(name, pass, client);  //RPL_JOIN , RPL_TOPIC, RPL_NAMREPLY, RPL_ENDOFNAMES
		}
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
	(void)	server;
	(void)	client;
	std::string token = v[0];
	std::string pong = "PONG server " + token + "\n";
	send(client.getFd(), pong.c_str(), pong.length(), 0);
}

void	Command::ping(Server &server, Client &client, std::vector<std::string> &v)
{
	(void)server;
	(void)client;
	(void)v;

	std::cout << "Command detected: PING" << std::endl;
	std::cout << "Parameters: " << std::endl;
	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it)
	{
		std::cout << *it << std::endl;
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

