#include "Client.hpp"

Client::Client() : _fd(-1), _isRegister(false), _passTaken(false), _nickname(""), _user(""), _startReg(time(0))
{
}

Client::Client(int fd) : _fd(fd), _isRegister(false), _passTaken(false), _nickname(""), _user(""), _startReg(time(0))
{
	std::cout << "New user" << std::endl;
}

Client &Client::operator=(const Client &obj)
{
	_fd = obj._fd;
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

const time_t & Client::getStartReg() const{return _startReg;}

const int &Client::getFd() const{return _fd;}

const std::string &Client::getBuffer() const {return _buffer;}

void Client::setBuffer(const std::string &buffer) {_buffer = buffer;}
