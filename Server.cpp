#include "Server.hpp"

Server::Server(const std::string &port, const std::string &psw) : _port(portConverter(port)), _psw(psw), _isPassword(psw.compare("") != 0)
{

}

Server::~Server()
{
}

void	Server::run()
{
	int serverSocket, newSocket;
	struct sockaddr_in serverAddr, newAddr;
	socklen_t addrSize;
	char buffer[512];

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) == -1)
		throw (std::runtime_error("fcntl-server"));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(this->_port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	listen(serverSocket, MAX_QUEUE_CONN);


	std::cout << "Server is listening on port " << this->_port << "..." << std::endl;

	int epoll_fd = epoll_create1(0); // Create an epoll instance

	struct epoll_event event;
	event.events = EPOLLIN; // Monitor read events
	event.data.fd = serverSocket;

	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverSocket, &event); // Add the server socket to the epoll

	struct epoll_event arrEvent[MAX_CLIENT]; // Create an event array to store events

	while (true)
	{
		int ready_fds = epoll_wait(epoll_fd, arrEvent, MAX_CLIENT, -1); // Wait for events

		for (int i = 0; i < ready_fds; ++i)
		{
			if (arrEvent[i].data.fd == serverSocket)
			{
				newSocket = accept(serverSocket, (struct sockaddr*)&newAddr, &addrSize);
				if (fcntl(newSocket, F_SETFL, O_NONBLOCK) == -1)
					throw (std::runtime_error("fcntl-client"));
				event.data.fd = newSocket;
				event.events = EPOLLIN; // Monitor read events for the new socket
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, newSocket, &event);
				std::cout << "Connection established with a client." << std::endl;
				Client tmp(newSocket);
				_clients[newSocket] = tmp;
			}
			else
			{
				int clientSocket = arrEvent[i].data.fd;
				memset(buffer, 0, sizeof(buffer));
				int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
				std::cout << "Client: " << clientSocket << std::endl;
				std::cout << buffer << std::endl;
				if (bytesReceived <= 0)
				{
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clientSocket, NULL);
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
	close(serverSocket);
}

void	Server::msgAnalyzer(Client &client, char *message)
{
	std::string msg = message;

	if (client.getIsRegistered())
		cmdAnalyzer(client, message);
	else
		registration(client, msg);
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
		if (!info.compare(_psw))
			client.setPassTaken(true);
	}
	else if (!token.compare("NICK"))
		client.setNikcname(info);
	else if (!token.compare("USER"))
		client.setUser(info);
	else if (!token.compare("TEST"))
		sleep(16);
	if (!client.getNickname().empty() && !client.getNickname().empty() && client.getPassTaken())
		client.setIsRegistered(true);
}

void	Server::cmdAnalyzer(Client &client, const std::string &msg)
{
 (void) client;
 (void) msg;
}
