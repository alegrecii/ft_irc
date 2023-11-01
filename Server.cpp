#include "Server.hpp"
#include "Server.hpp"

Server::Server(const std::string &port, const std::string &psw) : _port(portConverter(port)), _psw(psw), _isPassword(psw.compare("") != 0)
{
	_commands["JOIN"] = Command::join;
	_commands["PRIVMSG"] = Command::privmsg;
	_commands["PING"] = Command::ping;
	_commands["KICK"] = Command::kick;
	_commands["INVITE"] = Command::invite;
	_commands["TOPIC"] = Command::topic;
	_commands["MODE"] = Command::mode;
}

Server::~Server()
{
}

void	Server::run()
{
	signal(SIGINT, sigHandler);
	int serverSocket, newSocket;
	struct sockaddr_in serverAddr, newAddr;
	socklen_t addrSize;
	char buffer[512];

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
		perror("socket");
	int opt = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		perror("setsockopt");
	if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) == -1)
		throw (std::runtime_error("fcntl-server"));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(this->_port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)))
		perror("bind");
	if (listen(serverSocket, MAX_QUEUE_CONN) == -1)
		perror("listen");

	std::cout << "Server is listening on port " << this->_port << "..." << std::endl;

	int epoll_fd = epoll_create1(0); // Create an epoll instance

	struct epoll_event event;
	event.events = EPOLLIN; // Monitor read events
	event.data.fd = serverSocket;

	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverSocket, &event); // Add the server socket to the epoll

	struct epoll_event arrEvent[MAX_CLIENT]; // Create an event array to store events

	while (running)
	{
		int ready_fds = epoll_wait(epoll_fd, arrEvent, MAX_CLIENT, -1); // Wait for events

		for (int i = 0; i < ready_fds; ++i)
		{
			if (arrEvent[i].data.fd == serverSocket)
			{
				newSocket = accept(serverSocket, (struct sockaddr*)&newAddr, &addrSize);
				if (newSocket == -1)
					perror("accept");
				if (fcntl(newSocket, F_SETFL, O_NONBLOCK) == -1)
					throw (std::runtime_error("fcntl-client"));
				event.data.fd = newSocket;
				event.events = EPOLLIN; // Monitor read events for the new socket
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, newSocket, &event);
				std::cout << "Connection established with a client." << std::endl;
				// Client tmp(newSocket);
				// _clients[newSocket] = tmp;
				_clients.insert(std::make_pair(newSocket, Client(newSocket)));
			}
			else
			{
				int clientSocket = arrEvent[i].data.fd;
				memset(buffer, 0, sizeof(buffer));
				int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
				std::cout << "Client: " << clientSocket << std::endl;
				// std::cout << buffer << std::endl;
				if (bytesReceived <= 0)
				{
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clientSocket, NULL);
					_clients.erase(_clients.find(clientSocket));
					close(clientSocket);
					std::cout << "Client disconnected." << std::endl;
				}
				else
				{
					msgAnalyzer(_clients[clientSocket], buffer);
					//std::cout << "Client :" << buffer << std::endl;
				}
			}
		}
	}
	for(std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		std::cout << "Closing fd " << it->first << std::endl;
		send(it->first, "QUIT :Server disconnected!\r\n", 29, 0);
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, it->first, NULL);
		close(it->first);
	}
	close(epoll_fd);
	close(serverSocket);
}

void	Server::msgAnalyzer(Client &client, const char *message)
{
	std::string msg = message;
	size_t		pos;

	msg = client.getBuffer() + msg;
	client.setBuffer("");

	while ((pos = msg.find('\n')) != std::string::npos)
	{
		std::string			line;
		std::istringstream	iss(msg);

		std::getline(iss, line);
		msg.erase(0, pos + 1);

		if (client.getIsRegistered())
			cmdAnalyzer(client, line);
		else
			registration(client, line);

	}
	client.setBuffer(msg);
}

void	Server::welcomeMessage(Client &client)
{
	std::string serverName = ":SUCA";
	const int flags = MSG_DONTWAIT | MSG_NOSIGNAL;

    /* std::string motm = "\r\"
	"\r\n"
				 " /  |  /  | /      \\       /      |/       \\  /      \r\n"
                 ":$$ |  $$ |/$$$$$$  |      $$$$$$/ $$$$$$$  |/$$$$$$  |\r\n"
                 ":$$ |__$$ |$$____$$ |        $$ |  $$ |__$$ |$$ |  $$/\r\n"
                 ":$$    $$ | /    $$/         $$ |  $$    $$< $$ |      \r\n"
                 ":$$$$$$$$ |/$$$$$$/          $$ |  $$$$$$$  |$$ |   __ \r\n"
                 ":      $$ |$$ |_____        _$$ |_ $$ |  $$ |$$ \\__/  |\r\n"
                 ":      $$ |$$       |      / $$   |$$ |  $$ |$$    $$/ \r\n"
                 ":      $$/ $$$$$$$$/       $$$$$$/ $$/   $$/  $$$$$$/  \r\n"
	"\r\n"; */

	std::string RPL_WELCOME = serverName + " 001 " + client.getNickname() + " :Welcome to the 42 Internet Relay Network " + client.getNickname() + "\r\n";
	std::string RPL_YOURHOST = serverName + " 002 " + client.getNickname() + " :Hosted by Ale, Dami, Manu\r\n";
	std::string RPL_CREATED = serverName + " 003 " + client.getNickname() + " :This server was created in Nidavellir\r\n";

	send(client.getFd(), RPL_WELCOME.c_str(), RPL_WELCOME.length(), flags);
	send(client.getFd(), RPL_YOURHOST.c_str(), RPL_YOURHOST.length(), flags);
	send(client.getFd(), RPL_CREATED.c_str(), RPL_CREATED.length(), flags);

	// std::string RPL_CHANNEL = ":" + client.getNickname() + " JOIN #mdipaol\r\n";
	// std::string RPL_TOPIC = " 332 " + client.getNickname() + " #mdipaol" + " :Python is trash\r\n";
	// std::string RPL_LISTUSR = " 353 " + client.getNickname() + " #  mdipaol :~popo ~boh @jgw\r\n";
	// std::string	RPL_ENDOFLIST = " 366 " + client.getNickname() + " #mdipaol" + " :pipi\r\n";

	// std::string RPL_CREATION = serverName + " 329 " + client.getNickname() + " CIAO" + " wo\r\n";

	// send(client.getFd(), RPL_CHANNEL.c_str(), RPL_CHANNEL.length(), flags);
	// send(client.getFd(), RPL_TOPIC.c_str(), RPL_TOPIC.length(), flags);
	// send(client.getFd(), RPL_LISTUSR.c_str(), RPL_LISTUSR.length(), flags);
	// send(client.getFd(), RPL_ENDOFLIST.c_str(), RPL_ENDOFLIST.length(), flags);

	// send(client.getFd(), RPL_CREATION.c_str(), RPL_CREATION.length(), flags);
}

void	Server::registration(Client &client, const std::string &msg)
{
	std::istringstream iss(msg);
	std::string token;
	std::string info;

	iss >> token >> info;
	if (!_isPassword)
		client.setPassTaken(true);
	if (!token.compare("PASS"))
	{
		if (!info.compare(":" + _psw))
			client.setPassTaken(true);
	}
	else if (!token.compare("NICK"))
		client.setNikcname(info);
	else if (!token.compare("USER"))
		client.setUser(info);
	else if (!token.compare("TEST"))
		sleep(15);
	if (!client.getNickname().empty() && !client.getUser().empty() && client.getPassTaken())
	{
		std::cout << client.getNickname() << " registered!" << std::endl;
		//send(client.getFd(), "Registration finished!\r\n", 25, 0);
		//send(client.getFd(), "Welcome to My IRC Server! Enjoy your stay.\r\n", 45, 0);
		std::cout << "ciao" << std::endl;
		client.setIsRegistered(true);
		welcomeMessage(client);
	}
	std::cout << msg << std::endl;
}

static void	fillParam(std::vector<std::string> &vParam, std::istringstream &iss)
{
	std::string	param, last;
	

	while (std::getline(iss, param, ' '))
	{
		if (param.empty())
			continue;
		else if (param[0] == ':')
		{
			std::getline(iss, last, (char)EOF);
			param.erase(0, 1);
			if (!last.empty())
				vParam.push_back(param + " " + last);
		}
		else
			vParam.push_back(param);
	}
}

void	Server::cmdAnalyzer(Client &client, const std::string &msg)
{
	// pulire stringa /r/n
	std::vector<std::string>						vParam;
	std::string										cmd;
	std::istringstream								iss(msg);
	std::map<std::string, commandFunct>::iterator	it;

	std::cout << "\033[32m" << msg << "\033[0m" << std::endl;
	iss >> cmd;
	if ((it = _commands.find(cmd)) != _commands.end())
	{
		fillParam(vParam, iss);
		_commands[cmd](*this, client, vParam);
	}
	else
	{
		std::cout << "Unrecognized command" << std::endl;
	}
}

void	Server::setChannels(const std::string &name, const std::string &pass, const std::string &nameClient)
{
	if (_channels.find(name) == _channels.end())
	{
		// Channel channel(name, pass, nameClient);
		// _channels[name] = channel;
		_channels.insert(std::make_pair(name, Channel(name, pass, nameClient)));
	}
	else
	{
		_channels[name].setClients(name);
	}
}
