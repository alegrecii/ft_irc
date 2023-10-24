#include "Server.hpp"

Server::Server(const std::string &port, const std::string &pwd) : _port(port), _pwd(pwd)
{

}

Server::~Server()
{
}

void	Server::run()
{
	int serverSocket, newSocket;
	struct sockaddr_in serverAddr, newAddr;
	// socklen_t addrSize;
	// char buffer[1024];

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(this->_port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	listen(serverSocket, MAX_QUEUE_CONN);

	std::cout << "Server is listening on port " << this->_port << "..." << std::endl;
	
}
