#include "Channel.hpp"

Channel::Channel()
{
}

Channel::Channel(const std::string &name, Client *creator)
: _name(toLowerString(name)), _passKey(""), _inviteOnly(false), _topicRestrict(false) , _clientsLimit(0)
{
	if (creator)
		_clientsOp[creator->getNickname()] = creator;
	time_t c = time(NULL);
	std::stringstream ss;
	ss << c;
	_creationTime = ss.str();
}

Channel::~Channel()
{
}

Channel& Channel::operator=(const Channel& obj)
{
	this->_name = obj._name;
	this->_passKey = obj._passKey;

	return *this;
}

const std::string &Channel::getName() const{ return _name;}

const std::string &Channel::getTopic() const { return _topic; }

const std::string &Channel::getPasskey() const { return _passKey; }

bool Channel::getInviteOnly() const {return _inviteOnly;}

bool Channel::getTopicRestrict() const { return _topicRestrict; }

int Channel::getLimit() const { return _clientsLimit; }

void Channel::inviteHere(Client *client)
{
	if (std::find(_invitedClients.begin(), _invitedClients.end(), client) == _invitedClients.end())
		_invitedClients.push_back(client);
}

bool Channel::isInvited(Client *client) const
{
	if (std::find(_invitedClients.begin(), _invitedClients.end(), client) == _invitedClients.end())
		return false;
	return true;
}

void Channel::removeFromInvited(Client *client)
{
	_invitedClients.erase(std::find(_invitedClients.begin(), _invitedClients.end(), client));
}

void Channel::setInviteOnly(bool plus, Client &client)
{
	_inviteOnly = plus;
	char sign;
	if (plus)
		sign = '+';
	else
		sign = '-';
	std::string MODE_I = ":" + client.getNickname() + "!" + client.getUser() + "@localhost MODE " + _name + " " + sign + "i\r\n";
	sendToAll(MODE_I);
}

void Channel::setClients(Client *client)
{
	if (!client)
		return;
	_clients[client->getNickname()] = client;
}

void Channel::setTopicRestrict(bool plus, Client &client)
{
	_topicRestrict = plus;
	char sign;
	if (plus)
		sign = '+';
	else
		sign = '-';
	std::string MODE_T = ":" + client.getNickname() + "!" + client.getUser() + "@localhost MODE " + _name + " " + sign + "t\r\n";
	sendToAll(MODE_T);
}

void Channel::setTopic(const std::string &newTopic){ _topic = newTopic; }

void Channel::setPass(bool plus, Client &client, std::string pass)
{
	if (!plus && !_passKey.compare(""))
		return ;

	std::string RPL_PASS;
	if (!plus)
	{
		_passKey = "";
		RPL_PASS = ":" + client.getNickname() + "!" + client.getUser() + "@localhost MODE " + _name + " " + "-k\r\n";
	}
	else if (pass.find('.') == std::string::npos && pass.find(',') == std::string::npos)
	{
		_passKey = pass;
		RPL_PASS = ":" + client.getNickname() + "!" + client.getUser() + "@localhost MODE " + _name + " +k " + pass + "\r\n";
	}
	else
	{
		std::string ERR_PASS = "525 " + client.getNickname() + " " + _name + " :password not accepted (do not use '.' and ',')\r\n";
		send (client.getFd(), ERR_PASS.c_str(), ERR_PASS.size(), 0);
	}
	sendToAll(RPL_PASS);
}

void Channel::setOperator(bool plus, Client &client, std::string nick)
{
	if (!client.getNickname().compare(nick) && plus)
		return ;
	Client *target = findClient(nick);
	std::string RPL_OP;
	if (!target)
	{
		std::string	ERR_NOTONCHANNEL = "441 " + client.getNickname() + " " + nick + " " + _name + " :user is not in this channel\r\n";
		send(client.getFd(), ERR_NOTONCHANNEL.c_str(), ERR_NOTONCHANNEL.size(), 0);
		return;
	}
	if (plus)
	{
		if (!isOperator(nick))
		{
			_clients.erase(nick);
			_clientsOp[nick] = target;
		}
		RPL_OP = ":" + client.getNickname() + "!" + client.getUser() + "@localhost MODE " + _name + " +o " + nick + "\r\n";
	}
	else
	{
		if (isOperator(nick))
		{
			_clientsOp.erase(nick);
			_clients[nick] = target;
		}
		RPL_OP = ":" + client.getNickname() + "!" + client.getUser() + "@localhost MODE " + _name + " -o " + nick + "\r\n";
	}
	sendToAll(RPL_OP);
}

void Channel::setLimit(Client &client, std::string n)
{
	if (n.empty() && _clientsLimit <= 0)
		return ;
	std::string RPL_LIMIT;
	if (n.empty())
	{
		_clientsLimit = 0;
		RPL_LIMIT = ":" + client.getNickname() + "!" + client.getUser() + "@localhost MODE " + _name + " -l\r\n";
	}
	else
	{
		int x = std::atoi(n.c_str());
		if (x <= 0)
			return ;
		_clientsLimit = x;
		std::stringstream ss;
		ss << x;
		RPL_LIMIT = ":" + client.getNickname() + "!" + client.getUser() + "@localhost MODE " + _name + " +l " + ss.str() + "\r\n";
	}
	sendToAll(RPL_LIMIT);
}

void Channel::printModes(Client &client) const
{
	std::string modes = " +";
	std::string param;

	if (_inviteOnly)
		modes += "i";
	if (_topicRestrict)
		modes += "t";
	if (_clientsLimit > 0)
	{
		modes += "l";
		std::stringstream ss;
		ss << _clientsLimit;
		param += ss.str() + " ";
	}
	if (_passKey.compare(""))
	{
		modes += "k";
		if (findClient(client.getNickname()))
			param += _passKey;
	}
	std::string RPL_MODES = "324 " + client.getNickname() + " " + _name + modes + " " + param + "\r\n" ;
	std::string RPL_TIME = "329 " + client.getNickname() + " " + _name + " " + _creationTime + "\r\n" ;
	send(client.getFd(), RPL_MODES.c_str(), RPL_MODES.size(), 0);
	send(client.getFd(), RPL_TIME.c_str(), RPL_TIME.size(), 0);
}

unsigned int	Channel::getSize() const
{
	return (_clients.size() + _clientsOp.size());
}

void Channel::sendToAll(const std::string &msg) const
{
	std::vector<Client *>	all = getAllClients();
	size_t					size = all.size();

	for (size_t i = 0; i < size; i++)
		send(all[i]->getFd(), msg.c_str(), msg.size(), 0);
}

void	Channel::updateNickInChannel(const std::string &old, const std::string &newName)
{
	std::cout << "UPDATE NICK IN CHANNEL CALLED" << std::endl;
	if (!findClient(old))
	{
		std::cout << "NO CLIENT IN CHANNEL" << std::endl;
		return;
	}

	Client * tmp = NULL;

	if (_clients.find(old) != _clients.end())
	{
		std::cout << "NO OPERATOR" << std::endl;
		tmp = _clients[old];
		_clients.erase(old);
		_clients[newName] = tmp;
		return;
	}
	if (_clientsOp.find(old) != _clientsOp.end())
	{
		std::cout << "OPERATOR" << std::endl;
		tmp = _clientsOp[old];
		_clientsOp.erase(old);
		_clientsOp[newName] = tmp;
		return;
	}
	std::cout << "FIND NOT WORKING" << std::endl;
}

void	Channel::deleteClientFromChannel(const std::string &nick)
{
	Client	*c = findClient(nick);

	if (!c)
		return;
	if (isOperator(nick))
		_clientsOp.erase(nick);
	else
		_clients.erase(nick);
	if (isInvited(c))
		removeFromInvited(c);
}

std::vector<Client*>	Channel::getAllClients() const
{
	std::vector<Client*>	vec;
	std::map<std::string, Client*>::const_iterator	it = _clients.begin();
	std::map<std::string, Client*>::const_iterator	end = _clients.end();

	for(; it != end; ++it)
	{
		vec.push_back(it->second);
	}

	it = _clientsOp.begin();
	end = _clientsOp.end();

	for(; it != end; ++it)
	{
		vec.push_back(it->second);
	}
	return (vec);
}

std::map<std::string, Client *> Channel::getClientsOp() const { return _clientsOp;}

Client *Channel::findClient(const std::string &cl) const
{
	if (_clientsOp.find(cl) != _clientsOp.end())
		return _clientsOp.find(cl)->second;
	if (_clients.find(cl) != _clients.end())
		return _clients.find(cl)->second;
	return NULL;
}

bool Channel::isOperator(const std::string &cl) { return (_clientsOp.find(cl) != _clientsOp.end()); }
