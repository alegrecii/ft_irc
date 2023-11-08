#include "Command.hpp"

void	Command::status(Server &server, Client &client, std::vector<std::string> &v)
{
	(void)client;
	(void)v;
	server.status();
}

void Command::user(Server &server, Client &client, std::vector<std::string> &v)
{
	(void)server;
	if(client.getIsRegistered())
	{
		std::string error = "462 " + client.getNickname() + " :Already registered\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}
	if (!v.size())
	{
		std::string error = "461 " + client.getNickname() + " USER :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}

	client.setUser(v[0]);
}

void Command::pass(Server &server, Client &client, std::vector<std::string> &v)
{
	if (v.size() < 1)
	{
		std::string error = "461 " + client.getNickname() + " PASS :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}
	if(client.getIsRegistered())
	{
		std::string error = "462 " + client.getNickname() + " :Already registered\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}
	if (v[0].compare(":" + server.getPassword()))
	{
		std::string error = "464 " + client.getNickname() + " :Password incorrect\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}
	client.setPassTaken(true);
}

void	Command::nick(Server &server, Client &client, std::vector<std::string> &v)
{
	if (v.size() < 1)
	{
		std::string error = "461 " + client.getNickname() + " NICK :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}
	if (server.getClient(v[0]))
	{
		std::string	ERR_NICKNAMEINUSE = "433 " + client.getNickname() + " " + v[0] + " :Nickname is already in use\r\n";
		send(client.getFd(), ERR_NICKNAMEINUSE.c_str(), ERR_NICKNAMEINUSE.size(), 0);
		return;
	}
	server.updateNick(client, v[0]);
}

void	Command::join(Server &server, Client &client, std::vector<std::string> &v)
{
	std::cout << "Command detected: JOIN con " << v.size() << " parameters" << std::endl;
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
		std::cout << "POOPOO" << std::endl;
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
			std::cout << "CIAO" << std::endl;
			if (name[0] == '#' || name[0] == '&')
			{
				pass = "";
				server.setChannels(name, pass, client);
				//RPL_JOIN , RPL_TOPIC, RPL_NAMREPLY, RPL_ENDOFNAMES
			}
		}
	}
}

void	Command::msgToClient(Server &s, Client &c, const std::string &targetClient, const std::string &msg)
{
	Client	*cl = s.getClient(targetClient);
	if (cl == NULL)
	{
		// Error : no client corresponding to the targetNick
		std::string ERR_NOSUCHNICK = "401 " + c.getNickname() + " " + targetClient + " :No such nick\r\n";
		send(c.getFd(), ERR_NOSUCHNICK.c_str(), ERR_NOSUCHNICK.size(), 0);
		return ;
	}
	std::string	MSG = ":" + c.getNickname() + " PRIVMSG " + targetClient + " :" + msg + "\r\n";
	send(cl->getFd(), MSG.c_str(), MSG.size(), 0);
}

void	Command::msgToChannel(Server &s, Client &c, const std::string &chName, const std::string &msg)
{
	Channel	*ch = s.getChannel(chName);

	if (!ch)
	{
		std::string	ERR_CANNOTSENDTOCHAN = ":" + c.getNickname() + " 404 " + c.getNickname() + " " + chName + " :Cannot send to channel\r\n";
		send(c.getFd(), ERR_CANNOTSENDTOCHAN.c_str(), ERR_CANNOTSENDTOCHAN.size(), 0);
		return;
	}

	std::vector<Client *>	allClients = ch->getAllClients();
	for(size_t i = 0; i < allClients.size(); i++)
	{
		if (allClients[i]->getNickname() != c.getNickname())
		{
			std::string	MSG_CHANNEL = ":" + c.getNickname() + " PRIVMSG " + chName + " :" + msg + "\r\n";
			send(allClients[i]->getFd(), MSG_CHANNEL.c_str(), MSG_CHANNEL.size(), 0);
		}
	}
}

void	Command::privmsg(Server &server, Client &client, std::vector<std::string> &v)
{
	std::cout << "Command detected: PRIVMSG" << std::endl;
	(void) server;
	if (v.size() < 2)
	{
		std::string error = "461 " + client.getNickname() + " PRIVMSG :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}
	std::vector<std::string>	params = ft_split(v[0], ',');
	std::string					msg = v[1];

	for(size_t i = 0; i < params.size(); i++)
	{
		if (params[i][0] == '#' || params[i][0] == '&')
			msgToChannel(server, client, params[i], msg);
		else
			msgToClient(server, client, params[i], msg);
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
	std::cout << "Command detected: TOPIC" << std::endl;
	(void) server;
	if (v.size() < 1)
	{
		std::string error = "461 " + client.getNickname() + " TOPIC :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}
	Channel *c = server.getChannel(v[0]);
	if (!c)
	{
		std::string	ERR_NOSUCHCHANNEL = "403 " + client.getNickname() + " " + v[0] + ":No such channel \r\n";
		send(client.getFd(), ERR_NOSUCHCHANNEL.c_str(), ERR_NOSUCHCHANNEL.size(), 0);
		return;
	}
	if (!c->findClient(client.getNickname()))
	{
		std::string	ERR_NOTONCHANNEL = "442 " + client.getNickname() + " " + v[0] + ":You're not on that channel \r\n";
		send(client.getFd(), ERR_NOTONCHANNEL.c_str(), ERR_NOTONCHANNEL.size(), 0);
		return;
	}
	if (v.size() == 1)
	{
		std::string RPL_TOPIC = ":ircserv 332 " + client.getNickname() + " " + c->getName() + " :" + c->getTopic() + "\r\n";
		send(client.getFd(), RPL_TOPIC.c_str(), RPL_TOPIC.size(), 0);
		return ;
	}
	if (c->getTopicRestrict() && !c->isOperator(client.getNickname()))
	{
		std::string	ERR_CHANOPRIVSNEEDED = "482 " + client.getNickname() + " " + v[0] + ":You're not channel operator \r\n";
		send(client.getFd(), ERR_CHANOPRIVSNEEDED.c_str(), ERR_CHANOPRIVSNEEDED.size(), 0);
		return;
	}
	if (v.size() == 2)
		c->setTopic(v[1]);
	std::string TOPIC =	":" + client.getNickname() + "!" + client.getUser() + "@localhost TOPIC " + c->getName() + " :" + c->getTopic() + "\r\n";
	c->sendToAll(TOPIC);
}

void	Command::mode(Server &server, Client &client, std::vector<std::string> &v)
{
	std::cout << "Command detected: MODE" << std::endl;
	if (v.size() < 2)
	{
		std::string error = "461 " + client.getNickname() + " TOPIC :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}
	Channel *c = server.getChannel(v[0]);
	if (!c)
	{
		std::string	ERR_NOSUCHCHANNEL = "403 " + client.getNickname() + " " + v[0] + ":No such channel \r\n";
		send(client.getFd(), ERR_NOSUCHCHANNEL.c_str(), ERR_NOSUCHCHANNEL.size(), 0);
		return;
	}
	if (!c->findClient(client.getNickname()))
	{
		std::string	ERR_NOTONCHANNEL = "442 " + client.getNickname() + " " + v[0] + ":You're not on that channel \r\n";
		send(client.getFd(), ERR_NOTONCHANNEL.c_str(), ERR_NOTONCHANNEL.size(), 0);
		return;
	}
	if (!c->isOperator(client.getNickname()))
	{
		std::string	ERR_CHANOPRIVSNEEDED = "482 " + client.getNickname() + " " + v[0] + ":You're not channel operator \r\n";
		send(client.getFd(), ERR_CHANOPRIVSNEEDED.c_str(), ERR_CHANOPRIVSNEEDED.size(), 0);
		return;
	}
	// salvare parametro 1 in una stringa
	// variabile bool sul segno, parte da +
	// variabile int che scorre i parametri del vettore
	// check sulla stringa su quanti parametri ha bisogno e sui char, se mon sono quelli implementati da noi lanciare RPL 472 e uscire
	//se i parametri sono < di quel mumero uscire subito e dare "[19:42] [Error] MODE: This command requires more parameters" e uscire.
	//scorrere la stringa : se si trova un segno cambiare la variabile bool, se si trova una delle lettere che noi vogliamo far partire il mode(se questo ha bisogno di un argomento fare ++ sulla variabile counter)
	/*by AleGreci*/
}


