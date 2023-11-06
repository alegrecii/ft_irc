#include "Channel.hpp"

Channel::Channel()
{
}

Channel::Channel(const std::string &name, const std::string &pass, Client *creator)
: _name(name), _passKey(pass)
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

const std::string & Channel::getPasskey() const { return _passKey;}


void Channel::setClients(Client *client)
{
	if (!client)
		return;
	_clients[client->getNickname()] = client;
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