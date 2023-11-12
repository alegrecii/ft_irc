#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include <iomanip>
#include <limits>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <fcntl.h>
#include <stdio.h>
#include <csignal>

#include "Client.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Command.hpp"
#define MAX_QUEUE_CONN 10
#define MAX_CLIENT 1024

extern bool	running;

std::vector<std::string>	ft_split(const std::string &msg, const char delimiter);

uint16_t					portConverter(const std::string &port);
void						sigHandler(int);
std::string					toLowerString(std::string string);
bool						compareInsensitive(const std::string &a, const std::string &b);
