#include "Channel.hpp"

Channel::Channel()
{
}

Channel::Channel(const std::string &name, const std::string &pass, Client *creator)
: _name(name), _passKey(pass), _topicRestrict(false)
{
	if (creator)
		_clientsOp[creator->getNickname()] = creator;
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

bool Channel::getTopicRestrict() const { return _topicRestrict; }

void Channel::setClients(Client *client)
{
	if (!client)
		return;
	_clients[client->getNickname()] = client;
}

void Channel::setTopic(const std::string &newTopic){ _topic = newTopic; }

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

Client *Channel::findClient(const std::string &cl)
{
	if (_clientsOp.find(cl) != _clientsOp.end())
		return _clientsOp.find(cl)->second;
	if (_clients.find(cl) != _clients.end())
		return _clients.find(cl)->second;
	return NULL;
}

bool Channel::isOperator(const std::string &cl) { return (_clientsOp.find(cl) != _clientsOp.end()); }
