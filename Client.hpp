#pragma once

#include "utils.hpp"

class Channel;
class Server;

class Client
{
private:
	int							_fd;
	bool						_isRegister;
	bool						_passTaken;
	std::string					_nickname;
	std::string					_user;
	const time_t				_startReg;
	std::string					_buffer;

	std::vector<Channel *>		_joinedChannels;
public:
	Client();
	Client(int fd);
	Client &operator=(const Client &obj);
	~Client();

	bool				getIsRegistered() const;
	void				setIsRegistered(bool reg);
	bool				getPassTaken() const;
	void				setPassTaken(bool pass);
	const std::string	&getNickname() const;
	void				setNikcname(const std::string &nickname);
	const std::string	&getUser()const;
	void				setUser(const std::string &user);
	const time_t		&getStartReg() const;
	const int			&getFd() const;
	const std::string	&getBuffer() const;
	void				setBuffer(const std::string &buffer);

	void				addChannel(Channel *channel);
	const std::vector<Channel *>		&getJoinedChannels() const;
	void				deleteFromChannels(Server &server);
};

