#include "Client.hpp"

Client::Client() : _fd(-1), _isRegister(true), _passTaken(true), _nickname("PALLE"), _user("Palle"), _startReg(time(0))
{
}

Client::Client(int fd) : _fd(fd), _isRegister(false), _passTaken(false), _nickname(""), _user(""), _startReg(time(0))
{
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

const std::vector<Channel *>	&Client::getJoinedChannels() const { return _joinedChannels; }

void	Client::addChannel(Channel *channel)
{
	if (!channel)
		return;
	if (std::find(_joinedChannels.begin(), _joinedChannels.end(), channel) == _joinedChannels.end())
	{
		_joinedChannels.push_back(channel);
	}
}

void Client::setBuffer(const std::string &buffer) {_buffer = buffer;}

void Client::deleteFromChannels(Server &server)
{
	Channel *tmpCh = NULL;

	for(std::vector<Channel *>::iterator it = _joinedChannels.begin(); it != _joinedChannels.end(); ++it)
	{
		if(*it)
		{
			tmpCh = *it;
			(tmpCh)->deleteClientFromChannel(_nickname);
			if (!tmpCh->getSize())
				server.deleteChannel(tmpCh->getName());
			else
			{
				std::string RPL_PART = ":" + _nickname + "!" + _user + "@localhost QUIT :" + _nickname + " disconnected from the server\r\n";
				tmpCh->sendToAll(RPL_PART);
			}
		}
	}
}
