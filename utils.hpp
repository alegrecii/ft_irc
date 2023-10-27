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
#include <algorithm>
#include <sstream>
#include <ctime>
#include <fcntl.h>
#include <stdio.h>
#include <csignal>

#include "Client.hpp"
#include "Server.hpp"

#define MAX_QUEUE_CONN 10
#define MAX_CLIENT 1024

extern bool	running;

uint16_t	portConverter(const std::string &port);
void		sigHandler(int);