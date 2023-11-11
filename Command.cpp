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

	if (!ch->findClient(c.getNickname()))
	{
		std::string	ERR_NOTONCHANNEL = "442 " + c.getNickname() + " " + chName + " :You're not on that channel \r\n";
		send(c.getFd(), ERR_NOTONCHANNEL.c_str(), ERR_NOTONCHANNEL.size(), 0);
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
	if (v.size() < 2)
	{
		std::string error = "461 " + client.getNickname() + " KICK :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}
	Channel *ch = server.getChannel(v[0]);
	if (!ch)
	{
		std::string	ERR_NOSUCHCHANNEL = "403 " + client.getNickname() + " " + v[0] + " :No such channel \r\n";
		send(client.getFd(), ERR_NOSUCHCHANNEL.c_str(), ERR_NOSUCHCHANNEL.size(), 0);
		return;
	}
	if (!ch->findClient(client.getNickname()))
	{
		std::string	ERR_NOTONCHANNEL = "442 " + client.getNickname() + " " + v[0] + " :You're not on that channel \r\n";
		send(client.getFd(), ERR_NOTONCHANNEL.c_str(), ERR_NOTONCHANNEL.size(), 0);
		return;
	}
	if (!ch->isOperator(client.getNickname()))
	{
		std::string	ERR_CHANOPRIVSNEEDED = "482 " + client.getNickname() + " " + v[0] + " :You're not channel operator \r\n";
		send(client.getFd(), ERR_CHANOPRIVSNEEDED.c_str(), ERR_CHANOPRIVSNEEDED.size(), 0);
		return;
	}
	std::string	userToBan = v[1];
	Client	*toBan = ch->findClient(userToBan);
	if (!toBan)
	{
		std::string ERR_USERNOTINCHANNEL = "441 " + client.getNickname() + " " + userToBan + " " + ch->getName() + " :User not in this channel.\r\n";
		send(client.getFd(), ERR_USERNOTINCHANNEL.c_str(), ERR_USERNOTINCHANNEL.size(), 0);
		return ;
	}
	// KICK MESSAGE TO CLIENTS IN CHANNEL
	std::string KICK_MSG = ":" + client.getNickname() + "!" + client.getUser() + "@localhost KICK " + ch->getName() + " " + userToBan + "\r\n";
	ch->sendToAll(KICK_MSG);
	ch->deleteClientFromChannel(userToBan);
	// send(client.getFd(), KICK_MSG.c_str(), KICK_MSG.size(), 0);
}

void	Command::invite(Server &server, Client &client, std::vector<std::string> &v)
{
	if (v.size() < 2)
	{
		std::string error = "461 " + client.getNickname() + " INVITE :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}

	std::string	nickToInvite = v[0];
	std::string channel = v[1];

	Client	*toInvite = server.getClient(nickToInvite);
	Channel *toJoin = server.getChannel(channel);

	if (!toInvite)
	{
		std::string ERR_NOSUCHNICK = "401 " + client.getNickname() + " " + nickToInvite + " :No such nick\r\n";
		send(client.getFd(), ERR_NOSUCHNICK.c_str(), ERR_NOSUCHNICK.size(), 0);
		return ;
	}
	if (!toJoin)
	{
		std::string	ERR_NOSUCHCHANNEL = "403 " + client.getNickname() + " " + channel + " :No such channel \r\n";
		send(client.getFd(), ERR_NOSUCHCHANNEL.c_str(), ERR_NOSUCHCHANNEL.size(), 0);
		return ;
	}
	if (!toJoin->findClient(client.getNickname()))
	{
		std::string	ERR_NOTONCHANNEL = "442 " + client.getNickname() + " " + channel + " :You're not on that channel \r\n";
		send(client.getFd(), ERR_NOTONCHANNEL.c_str(), ERR_NOTONCHANNEL.size(), 0);
		return;
	}
	if (!toJoin->isOperator(client.getNickname()))
	{
		std::string	ERR_CHANOPRIVSNEEDED = "482 " + client.getNickname() + " " + channel + " :You're not channel operator \r\n";
		send(client.getFd(), ERR_CHANOPRIVSNEEDED.c_str(), ERR_CHANOPRIVSNEEDED.size(), 0);
		return;
	}
	if (toJoin->findClient(toInvite->getNickname()))
	{
		std::string ERR_USERONCHANNEL = "443 " + client.getNickname() + " " + nickToInvite + " " + toJoin->getName() + " :is already on channel\r\n";
		send(client.getFd(), ERR_USERONCHANNEL.c_str(), ERR_USERONCHANNEL.size(), 0);
		return;
	}

	std::string INVITE_MSG = ":" + client.getNickname() + "!" + client.getUser() + "@localhost INVITE " + nickToInvite + " " + channel + "\r\n";
	send(toInvite->getFd(), INVITE_MSG.c_str(), INVITE_MSG.size(), 0);
	toJoin->inviteHere(toInvite);
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
		std::string	ERR_NOSUCHCHANNEL = "403 " + client.getNickname() + " " + v[0] + " :No such channel \r\n";
		send(client.getFd(), ERR_NOSUCHCHANNEL.c_str(), ERR_NOSUCHCHANNEL.size(), 0);
		return;
	}
	if (!c->findClient(client.getNickname()))
	{
		std::string	ERR_NOTONCHANNEL = "442 " + client.getNickname() + " " + v[0] + " :You're not on that channel \r\n";
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
		std::string	ERR_CHANOPRIVSNEEDED = "482 " + client.getNickname() + " " + v[0] + " :You're not channel operator \r\n";
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
	if (!v.size())
	{
		std::string error = "461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}
	Channel *c = server.getChannel(v[0]);
	if (!c)
	{
		std::string	ERR_NOSUCHCHANNEL = "403 " + client.getNickname() + " " + v[0] + " :No such channel \r\n";
		send(client.getFd(), ERR_NOSUCHCHANNEL.c_str(), ERR_NOSUCHCHANNEL.size(), 0);
		return;
	}
	//TODO:check mode con solo canale (v.size < 2)
	if (v.size() < 2)
	{
		c->printModes(client);
		return;
	}
	if (!v[1].compare("+b"))
		return;
	if (!c->findClient(client.getNickname()))
	{
		std::string	ERR_NOTONCHANNEL = "442 " + client.getNickname() + " " + v[0] + " :You're not on that channel \r\n";
		send(client.getFd(), ERR_NOTONCHANNEL.c_str(), ERR_NOTONCHANNEL.size(), 0);
		return;
	}
	if (!c->isOperator(client.getNickname()))
	{
		std::string	ERR_CHANOPRIVSNEEDED = "482 " + client.getNickname() + " " + v[0] + " :You're not channel operator \r\n";
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

	std::string modes = v[1];
	bool plus = true;
	int	paramCounter = 0;
	for (size_t i = 0; modes[i];  i++)
	{
		if(modes[i] == '-')
			plus = false;
		else if(modes[i] == '+')
			plus = true;
		else if (modes[i] == 'o' || modes[i] == 'k' || (modes[i] == 'l' && plus))
			paramCounter++;
		else if (modes[i] != 'i' && modes[i] != 't' && modes[i] != 'l')
			{
				std::string	ERR_UNKNOWNMODE = "472 " + client.getNickname() + " " + modes[i] + " :is unknown mode char to me \r\n";
				send(client.getFd(), ERR_UNKNOWNMODE.c_str(), ERR_UNKNOWNMODE.size(), 0);
				return;
			}
	}
	if (paramCounter > static_cast<int>(v.size()) - 2)
	{
		std::string error = "461 " + client.getNickname() + " MODE :This command requires more parameters.\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}
	paramCounter = 2;
	for (size_t i = 0; modes[i];  i++)
	{
		if(modes[i] == '-')
			plus = false;
		else if(modes[i] == '+')
			plus = true;
		else if (modes[i] == 'i')
		{
			if (c->getInviteOnly() != plus)
				c->setInviteOnly(plus, client);
		}
 		else if (modes[i] == 't')
		{
			if (c->getTopicRestrict() != plus)
				c->setTopicRestrict(plus, client);
		}
		else if (modes[i] == 'k')
		{
				c->setPass(plus, client, v[paramCounter]);
				paramCounter++;
		}
		else if (modes[i] == 'o')
		{
			c->setOperator(plus, client, v[paramCounter]);
			paramCounter++;
		}
		else if (modes[i] == 'l')
		{
			if (plus)
			{
				c->setLimit(client, v[paramCounter]);
				paramCounter++;
			}
			else
				c->setLimit(client, "");

		}
		//...
	}

	// salvare parametro 1 in una stringa
	// variabile bool sul segno, parte da +
	// variabile int che scorre i parametri del vettore
	// check sulla stringa su quanti parametri ha bisogno e sui char, se mon sono quelli implementati da noi lanciare RPL 472 e uscire
	//se i parametri sono < di quel mumero uscire subito e dare "[19:42] [Error] MODE: This command requires more parameters" e uscire.
	//scorrere la stringa : se si trova un segno cambiare la variabile bool, se si trova una delle lettere che noi vogliamo far partire il mode(se questo ha bisogno di un argomento fare ++ sulla variabile counter)
	/*by AleGreci*/
}

void Command::part(Server &server, Client &client, std::vector<std::string> &v)
{
	std::cout << "Command detected: PART" << std::endl;
	if (v.size() < 1)
	{
		std::string error = "461 " + client.getNickname() + " PART :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}
	std::string param = v[0];
	std::vector<std::string> split = ft_split(param, ',');
	for(std::vector<std::string>::iterator it = split.begin(); it != split.end(); ++it)
	{
		std::string name = *it;
		if (name[0] == '#' || name[0] == '&')
		{
			std::cout << name << std::endl;
			Channel *c = server.getChannel(name);
			if (!c)
			{
				std::string	ERR_NOSUCHCHANNEL = "403 " + client.getNickname() + " " + v[0] + " :No such channel \r\n";
				send(client.getFd(), ERR_NOSUCHCHANNEL.c_str(), ERR_NOSUCHCHANNEL.size(), 0);
				return;
			}
			if (!c->findClient(client.getNickname()))
			{
				std::string	ERR_NOTONCHANNEL = "442 " + client.getNickname() + " " + v[0] + " :You're not on that channel \r\n";
				send(client.getFd(), ERR_NOTONCHANNEL.c_str(), ERR_NOTONCHANNEL.size(), 0);
				return;
			}
			std::string RPL_PART = ":" + client.getNickname() + "!" + client.getUser() + "@localhost PART " + c->getName() + "\r\n";
			c->sendToAll(RPL_PART);
			c->deleteClientFromChannel(client.getNickname());
			if (c->getSize() == 0)
				server.deleteChannel(c->getName());
		}
	}
}


void	Command::who(Server &server, Client &client, std::vector<std::string> &v)
{
	if (!v.size())
		return;

	Channel	*ch = NULL;
	Client	*c = NULL;

	std::string	mask = v[0];
	std::string	RPL_WHOREPLY;

	if (mask[0] == '#')
	{
		ch = server.getChannel(mask);
		if (!ch)
			return;
		RPL_WHOREPLY = ":ircserv 352 " + client.getNickname() + " " + ch->getName() + " " + client.getUser() + " host ircserv " + client.getNickname() + " H :0 " + client.getUser() + "\r\n";
	}
	else
	{
		c = server.getClient(mask);
		if (!c)
			return;
		RPL_WHOREPLY = ":ircserv 352 " + client.getNickname() + " * " + client.getUser() + " host ircserv " + client.getNickname() + " H :1 " + client.getUser() + "\r\n";
	}
	std::string	RPL_ENDOFWHO = ":ircserv 315 " + client.getNickname() + " " + mask + " :End of WHO list\r\n";
	send(client.getFd(), RPL_WHOREPLY.c_str(), RPL_WHOREPLY.size(), 0);
	send(client.getFd(), RPL_ENDOFWHO.c_str(), RPL_ENDOFWHO.size(), 0);
}

void	Command::userhost(Server &server, Client &client, std::vector<std::string> &v)
{
	if (!v.size())
	{
		std::string error = "461 " + client.getNickname() + " USERHOST :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		return;
	}

	std::string users;

	for (size_t i = 0; i < v.size() && i < 5; i++)
	{
		Client	*c = server.getClient(v[i]);
		if (c)
		{
			users += c->getNickname() + " ";
		}
	}

	std::string	RPL_USERHOST = ":ircserv 302 " + users + "\r\n";
	send(client.getFd(), RPL_USERHOST.c_str(), RPL_USERHOST.size(), 0);
}
	// 	std::string	ERR_CHANOPRIVSNEEDED = "482 " + client.getNickname() + " " + v[0] + ":You're not channel operator \r\n";
	// 	send(client.getFd(), ERR_CHANOPRIVSNEEDED.c_str(), ERR_CHANOPRIVSNEEDED.size(), 0);
	// 	return;
	// }


