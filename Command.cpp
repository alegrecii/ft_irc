#include "Command.hpp"

const std::string Command::_puns[10] = {
	"«Mi rifiuto!» disse il netturbino.",
	"Come si chiamano i boy-scout che vanno in macchina? Le giovani marmitte!",
	"Abbiamo riso abbastanza, adesso pasta!",
	"Se il cane ringhia, la ringhiera abbaia?!",
	"Le natiche degli eschimesi sono IGLOOTEI",
	"«Un uomo entra in un caffè». SPLASH",
	"La funga dice al fungo: «Sei un porcino!»",
	"Due mandarini litigano furiosamente e uno dice all'altro: «guarda che ti spicchio!!»",
	"Discorso tra pecore: «Stasera non riesco a dormire». «Prova a contare i pastori....»",
	"Qual è la pianta più puzzolente? Quella dei piedi!"
};

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
		send(client.getFd(), error.c_str(), error.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	if (!v.size())
	{
		std::string error = "461 " + client.getNickname() + " USER :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	client.setUser(v[0]);
}

void Command::pass(Server &server, Client &client, std::vector<std::string> &v)
{
	if (v.size() < 1)
	{
		std::string error = "461 " + client.getNickname() + " PASS :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	if(client.getIsRegistered())
	{
		std::string error = "462 " + client.getNickname() + " :Already registered\r\n";
		send(client.getFd(), error.c_str(), error.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	if (v[0].compare(server.getPassword()))
	{
		std::string error = "464 " + client.getNickname() + " :Password incorrect\r\n";
		send(client.getFd(), error.c_str(), error.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	client.setPassTaken(true);
}

void	Command::nick(Server &server, Client &client, std::vector<std::string> &v)
{
	if (v.size() < 1)
	{
		std::string error = "461 " + client.getNickname() + " NICK :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	if (inSet(v[0], "./;: !@#&?"))
	{
		std::string error = "432 " + client.getNickname() + " " + v[0] + " :Erroneus nickname\r\n";
		send(client.getFd(), error.c_str(), error.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	if (server.getClient(v[0]))
	{
		std::string	ERR_NICKNAMEINUSE = "433 " + client.getNickname() + " " + v[0] + " :Nickname is already in use\r\n";
		send(client.getFd(), ERR_NICKNAMEINUSE.c_str(), ERR_NICKNAMEINUSE.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	server.updateNick(client, v[0]);
}

std::string	Command::findUsers(Channel &channel)
{
	std::string						users;
	std::vector<Client *>			Clients = channel.getAllClients();
	std::map<std::string, Client *> Ops = channel.getClientsOp();

	for (std::vector<Client *>::iterator it = Clients.begin(); it != Clients.end(); ++it)
	{
		if (Ops.find((*it)->getNickname()) != Ops.end())
			users += "@" + (*it)->getNickname() + " ";
		else
			users += (*it)->getNickname() + " ";
	}
	return users;
}

void	Command::sendJoin(Channel &channel, Client &client)
{
	std::vector<Client *>	allClient = channel.getAllClients();
	std::string	users = findUsers(channel);
	std::string chName = channel.getName();
	//JOIN_RPL

	std::string RPL_JOIN = ":" + client.getNickname() + "!" + client.getUser() + "@localhost JOIN :" + chName + "\r\n";
	std::string RPL_NAMREPLY = ":ircserv 353 " + client.getNickname() + " = " + chName + " :" + users + "\r\n";
	std::string RPL_ENDOFNAMES = ":ircserv 366 " + client.getNickname() + " " + chName + " :End of NAMES list\r\n";

	for (std::vector<Client *>::iterator it = allClient.begin(); it != allClient.end(); ++it)
		send((*it)->getFd(), RPL_JOIN.c_str(), RPL_JOIN.size(), MSG_DONTWAIT | MSG_NOSIGNAL);

	//TOPIC_RPL

	std::string topic = channel.getTopic();
	if (!topic.empty())
	{
		std::string RPL_TOPIC = ":ircserv 332 " + client.getNickname() + " " + chName + " :" + topic + "\r\n";
		send(client.getFd(), RPL_TOPIC.c_str(), RPL_TOPIC.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
	}
	//LIST_RPL
	send(client.getFd(), RPL_NAMREPLY.c_str(), RPL_NAMREPLY.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
	send(client.getFd(), RPL_ENDOFNAMES.c_str(), RPL_ENDOFNAMES.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
}

void	Command::setChannels(Server &server, const std::string &chName, const std::string &pass, Client &client)
{
	std::string toLowerName = toLowerString(chName);
	Channel 	*ch = server.getChannel(toLowerName);

	if (!ch)
	{
		ch = new Channel(toLowerName, &client);
		if (!ch)
			return;
		server.addChannel(ch);
		client.addChannel(ch);
		sendJoin(*ch, client);
	}
	else
	{
		if (ch->findClient(client.getNickname()))
			return;
		if (!ch->getPasskey().compare("") || !ch->getPasskey().compare(pass))
		{
			if (ch->getLimit() <= 0 || ch->getLimit() > static_cast<int>(ch->getSize()))
			{
				if ((ch->getInviteOnly() && ch->isInvited(&client)) || !ch->getInviteOnly())
				{
					ch->setClients(&client);
					client.addChannel(ch);
					sendJoin(*ch, client);
					ch->removeFromInvited(&client);
				}
				else
				{
					std::string ERR_INVITEONLYCHAN = "473 " + client.getNickname() + " " + chName + " :Cannot join channel, InviteOnly channel!(+i)\r\n";
					send(client.getFd(), ERR_INVITEONLYCHAN.c_str(), ERR_INVITEONLYCHAN.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
				}
			}
			else
			{
				std::string ERR_CHANNELISFULL = "471 " + client.getNickname() + " " + chName + " :Cannot join channel, channel is full!(+l)\r\n";
				send(client.getFd(), ERR_CHANNELISFULL.c_str(), ERR_CHANNELISFULL.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
			}
		}
		else
		{
			std::string	ERR_BADCHANNELKEY = "475 " + client.getNickname() + " " + chName + " :Cannot join channel (+k)!\r\n";
			send(client.getFd(), ERR_BADCHANNELKEY.c_str(), ERR_BADCHANNELKEY.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		}

	}
}

void	Command::join(Server &server, Client &client, std::vector<std::string> &v)
{
	if (!v.size())
	{
		std::string error = "461 " + client.getNickname() + " JOIN :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	std::string	chName, pass;
	std::istringstream channels(v[0]);

	if (v.size() == 2)
	{
		std::istringstream passwords(v[1]);
		while(std::getline(channels, chName, ','))
		{
			if (chName[0] == '#' || chName[0] == '&')
			{
				if(!std::getline(passwords, pass, ','))
					pass = "";
				setChannels(server, chName, pass, client);
				//RPL_JOIN , RPL_TOPIC, RPL_NAMREPLY, RPL_ENDOFNAMES
			}
		}
	}
	else if (v.size() == 1)
	{
		while(std::getline(channels, chName, ','))
		{
			if (chName[0] == '#' || chName[0] == '&')
			{
				pass = "";
				setChannels(server, chName, pass, client);
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
		send(c.getFd(), ERR_NOSUCHNICK.c_str(), ERR_NOSUCHNICK.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return ;
	}
	std::string	MSG = ":" + c.getNickname() + "! PRIVMSG " + targetClient + " :" + msg + "\r\n";
	send(cl->getFd(), MSG.c_str(), MSG.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
}

void	Command::bot(Server &s, Channel *channel, const std::string &msg)
{
	if (channel->getName().compare(SUPERCHANNEL))
		return ;
	std::string	cmd = toLowerString( msg );
	std::string	toSend;

	if (!cmd.compare("!manu"))
	{
		toSend = "try these commands !manu <command> :\ndaii \neddaii \nbattuta";
		msgToChannel(s, *(channel->findClient("Manuel")), channel->getName() ,toSend);
		return;
	}
	if(cmd.compare(0, 6, "!manu "))
		return;
	cmd.erase(0, 6);

	if (!cmd.compare("daii"))
		toSend = "DAIIIIIII";
	else if (!cmd.compare("eddaii"))
		toSend = "EDDAIIIIIII";
	else if (!cmd.compare("battuta"))
		toSend = _puns[rand()%10];
	else
		toSend = "Non conosco que, c'ho piacere";
	msgToChannel(s, *(channel->findClient("Manuel")), channel->getName() ,toSend);
}

void	Command::msgToChannel(Server &s, Client &c, const std::string &chName, const std::string &msg)
{
	Channel	*ch = s.getChannel(chName);

	if (!ch)
	{
		std::string	ERR_CANNOTSENDTOCHAN = ":" + c.getNickname() + " 404 " + c.getNickname() + " " + chName + " :Cannot send to channel\r\n";
		send(c.getFd(), ERR_CANNOTSENDTOCHAN.c_str(), ERR_CANNOTSENDTOCHAN.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}

	if (!ch->findClient(c.getNickname()))
	{
		std::string	ERR_NOTONCHANNEL = "442 " + c.getNickname() + " " + chName + " :You're not on that channel \r\n";
		send(c.getFd(), ERR_NOTONCHANNEL.c_str(), ERR_NOTONCHANNEL.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}

	std::vector<Client *>	allClients = ch->getAllClients();
	for(size_t i = 0; i < allClients.size(); i++)
	{
		if (allClients[i]->getNickname() != c.getNickname())
		{
			std::string	MSG_CHANNEL = ":" + c.getNickname() + "! PRIVMSG " + chName + " :" + msg + "\r\n";
			send(allClients[i]->getFd(), MSG_CHANNEL.c_str(), MSG_CHANNEL.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		}
	}
	//BOT
	Command::bot(s, ch, msg);
}

void	Command::privmsg(Server &server, Client &client, std::vector<std::string> &v)
{
	if (v.size() < 2)
	{
		std::string error = "461 " + client.getNickname() + " PRIVMSG :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
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
		send(client.getFd(), pong.c_str(), pong.length(), MSG_DONTWAIT | MSG_NOSIGNAL);
	}
}

void	Command::kick(Server &server, Client &client, std::vector<std::string> &v)
{
	if (v.size() < 2)
	{
		std::string error = "461 " + client.getNickname() + " KICK :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	Channel *ch = server.getChannel(v[0]);
	if (!ch)
	{
		std::string	ERR_NOSUCHCHANNEL = "403 " + client.getNickname() + " " + v[0] + " :No such channel \r\n";
		send(client.getFd(), ERR_NOSUCHCHANNEL.c_str(), ERR_NOSUCHCHANNEL.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	if (!ch->findClient(client.getNickname()))
	{
		std::string	ERR_NOTONCHANNEL = "442 " + client.getNickname() + " " + v[0] + " :You're not on that channel \r\n";
		send(client.getFd(), ERR_NOTONCHANNEL.c_str(), ERR_NOTONCHANNEL.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	if (!ch->isOperator(client.getNickname()))
	{
		std::string	ERR_CHANOPRIVSNEEDED = "482 " + client.getNickname() + " " + v[0] + " :You're not channel operator \r\n";
		send(client.getFd(), ERR_CHANOPRIVSNEEDED.c_str(), ERR_CHANOPRIVSNEEDED.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	std::string	userToBan = v[1];
	Client	*toBan = ch->findClient(userToBan);
	if (!toBan)
	{
		std::string ERR_USERNOTINCHANNEL = "441 " + client.getNickname() + " " + userToBan + " " + ch->getName() + " :User not in this channel.\r\n";
		send(client.getFd(), ERR_USERNOTINCHANNEL.c_str(), ERR_USERNOTINCHANNEL.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return ;
	}
	// KICK MESSAGE TO CLIENTS IN CHANNEL
	std::string KICK_MSG = ":" + client.getNickname() + "!" + client.getUser() + "@localhost KICK " + ch->getName() + " " + userToBan + "\r\n";
	ch->sendToAll(KICK_MSG);
	ch->deleteClientFromChannel(userToBan);
	if (!ch->getSize())
		server.deleteChannel(ch->getName());
}

void	Command::invite(Server &server, Client &client, std::vector<std::string> &v)
{
	if (v.size() < 2)
	{
		std::string error = "461 " + client.getNickname() + " INVITE :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}

	std::string	nickToInvite = v[0];
	std::string channel = v[1];

	Client	*toInvite = server.getClient(nickToInvite);
	Channel *toJoin = server.getChannel(channel);

	if (!toInvite)
	{
		std::string ERR_NOSUCHNICK = "401 " + client.getNickname() + " " + nickToInvite + " :No such nick\r\n";
		send(client.getFd(), ERR_NOSUCHNICK.c_str(), ERR_NOSUCHNICK.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return ;
	}
	if (!toJoin)
	{
		std::string	ERR_NOSUCHCHANNEL = "403 " + client.getNickname() + " " + channel + " :No such channel \r\n";
		send(client.getFd(), ERR_NOSUCHCHANNEL.c_str(), ERR_NOSUCHCHANNEL.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return ;
	}
	if (!toJoin->findClient(client.getNickname()))
	{
		std::string	ERR_NOTONCHANNEL = "442 " + client.getNickname() + " " + channel + " :You're not on that channel \r\n";
		send(client.getFd(), ERR_NOTONCHANNEL.c_str(), ERR_NOTONCHANNEL.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	if (!toJoin->isOperator(client.getNickname()))
	{
		std::string	ERR_CHANOPRIVSNEEDED = "482 " + client.getNickname() + " " + channel + " :You're not channel operator \r\n";
		send(client.getFd(), ERR_CHANOPRIVSNEEDED.c_str(), ERR_CHANOPRIVSNEEDED.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	if (toJoin->findClient(toInvite->getNickname()))
	{
		std::string ERR_USERONCHANNEL = "443 " + client.getNickname() + " " + nickToInvite + " " + toJoin->getName() + " :is already on channel\r\n";
		send(client.getFd(), ERR_USERONCHANNEL.c_str(), ERR_USERONCHANNEL.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}

	std::string INVITE_MSG = ":" + client.getNickname() + "!" + client.getUser() + "@localhost INVITE " + nickToInvite + " " + channel + "\r\n";
	send(toInvite->getFd(), INVITE_MSG.c_str(), INVITE_MSG.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
	toJoin->inviteHere(toInvite);
}

void	Command::topic(Server &server, Client &client, std::vector<std::string> &v)
{
	(void) server;
	if (v.size() < 1)
	{
		std::string error = "461 " + client.getNickname() + " TOPIC :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	Channel *c = server.getChannel(v[0]);
	if (!c)
	{
		std::string	ERR_NOSUCHCHANNEL = "403 " + client.getNickname() + " " + v[0] + " :No such channel \r\n";
		send(client.getFd(), ERR_NOSUCHCHANNEL.c_str(), ERR_NOSUCHCHANNEL.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	if (!c->findClient(client.getNickname()))
	{
		std::string	ERR_NOTONCHANNEL = "442 " + client.getNickname() + " " + v[0] + " :You're not on that channel \r\n";
		send(client.getFd(), ERR_NOTONCHANNEL.c_str(), ERR_NOTONCHANNEL.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
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
		send(client.getFd(), ERR_CHANOPRIVSNEEDED.c_str(), ERR_CHANOPRIVSNEEDED.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	if (v.size() == 2)
		c->setTopic(v[1]);
	std::string TOPIC =	":" + client.getNickname() + "!" + client.getUser() + "@localhost TOPIC " + c->getName() + " :" + c->getTopic() + "\r\n";
	c->sendToAll(TOPIC);
}

void	Command::mode(Server &server, Client &client, std::vector<std::string> &v)
{
	if (!v.size())
	{
		std::string error = "461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	Channel *c = server.getChannel(v[0]);
	if (!c)
	{
		std::string	ERR_NOSUCHCHANNEL = "403 " + client.getNickname() + " " + v[0] + " :No such channel \r\n";
		send(client.getFd(), ERR_NOSUCHCHANNEL.c_str(), ERR_NOSUCHCHANNEL.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
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
		send(client.getFd(), ERR_NOTONCHANNEL.c_str(), ERR_NOTONCHANNEL.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	if (!c->isOperator(client.getNickname()))
	{
		std::string	ERR_CHANOPRIVSNEEDED = "482 " + client.getNickname() + " " + v[0] + " :You're not channel operator \r\n";
		send(client.getFd(), ERR_CHANOPRIVSNEEDED.c_str(), ERR_CHANOPRIVSNEEDED.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
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
				send(client.getFd(), ERR_UNKNOWNMODE.c_str(), ERR_UNKNOWNMODE.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
				return;
			}
	}
	if (paramCounter > static_cast<int>(v.size()) - 2)
	{
		std::string error = "461 " + client.getNickname() + " MODE :This command requires more parameters.\r\n";
		send(client.getFd(), error.c_str(), error.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
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
}

void Command::part(Server &server, Client &client, std::vector<std::string> &v)
{
	if (v.size() < 1)
	{
		std::string error = "461 " + client.getNickname() + " PART :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		return;
	}
	std::string param = v[0];
	std::vector<std::string> split = ft_split(param, ',');
	for(std::vector<std::string>::iterator it = split.begin(); it != split.end(); ++it)
	{
		std::string name = *it;
		if (name[0] == '#' || name[0] == '&')
		{
			Channel *c = server.getChannel(name);
			if (!c)
			{
				std::string	ERR_NOSUCHCHANNEL = "403 " + client.getNickname() + " " + v[0] + " :No such channel \r\n";
				send(client.getFd(), ERR_NOSUCHCHANNEL.c_str(), ERR_NOSUCHCHANNEL.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
				return;
			}
			if (!c->findClient(client.getNickname()))
			{
				std::string	ERR_NOTONCHANNEL = "442 " + client.getNickname() + " " + v[0] + " :You're not on that channel \r\n";
				send(client.getFd(), ERR_NOTONCHANNEL.c_str(), ERR_NOTONCHANNEL.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
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
	send(client.getFd(), RPL_WHOREPLY.c_str(), RPL_WHOREPLY.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
	send(client.getFd(), RPL_ENDOFWHO.c_str(), RPL_ENDOFWHO.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
}

void	Command::userhost(Server &server, Client &client, std::vector<std::string> &v)
{
	if (!v.size())
	{
		std::string error = "461 " + client.getNickname() + " USERHOST :Not enough parameters\r\n";
		send(client.getFd(), error.c_str(), error.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
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
	send(client.getFd(), RPL_USERHOST.c_str(), RPL_USERHOST.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
}

void	Command::help(Server &server, Client &client, std::vector<std::string> &v)
{
	(void)server;
	(void)v;
	std::string CMDS[12] = {"JOIN", "PART", "PRIVMSG", "PING", "KICK", "INVITE", "TOPIC", "MODE", "NICK", "PASS", "USER", "HELP"};
	std::string toSend;
	toSend = ":ircserv 704 > * :**Help System**\r\n";
	for (int i = 0; i < 12; ++i)
	{
		toSend += ":ircserv 705 > * :" + CMDS[i] + "\r\n";
	}
	toSend += ":ircserv 706 > * :**End of Help System**\r\n";
	send(client.getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
}