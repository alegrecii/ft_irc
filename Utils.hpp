#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include <iomanip>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include "Server.hpp"

#define MAX_QUEUE_CONN 10
