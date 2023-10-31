#include "Channel.hpp"

Channel::Channel()
{
}

Channel::Channel(const std::string &name, const std::string &pass, const std::string &nameClient) : _name(name), _passKey(pass)
{
	_clientsOp[nameClient] = true;
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

void	Channel::setClients(const std::string &name)
{
	_clientsOp[name] = false;
}
