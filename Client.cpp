#include "Client.hpp"

Client::Client() : _isRegister(false), _passTaken(false), _nickname(""), _user("")
{
}

Client &Client::operator=(const Client &obj)
{
	(void)obj;
	return *this;
}

Client::~Client()
{
}

bool Client::getIsRegistered() const {return _isRegister;}

void Client::setIsRegistered(bool reg){_isRegister = reg;}

bool Client::getPassTaken() const{return _passTaken;}

void Client::setPassTaken(bool pass){_passTaken = pass;}

const std::string &Client::getNickname() const{return _nickname;}

void Client::setNikcname(const std::string &nickname){_nickname = nickname;}

const std::string &Client::getUser() const{return _user;}

void Client::setUser(const std::string &user){_user = user;}
