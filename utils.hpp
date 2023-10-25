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

#include "Server.hpp"

#define MAX_QUEUE_CONN 10
#define MAX_CLIENT 1024

uint16_t	portConverter(const std::string &port);
