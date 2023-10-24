#pragma once

#include "Utils.hpp"

class Server
{
private:
	uint16_t const	_port;
	std::string				_psw;
public:
	Server(const std::string &port, const std::string &psw);
	~Server();


	void	run();
};

