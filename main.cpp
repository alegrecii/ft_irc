#include "utils.hpp"

bool	running;

int	main(int ac, char **av)
{
	running = true;
	if (ac != 3)
	{
		std::cerr << "Insert correct arguments!" << std::endl;
		std::cerr << "Usage: ./ircserv <port> <passsword>" << std::endl;
		return 1;
	}
	else
	{
		try
		{
			Server server(av[1], av[2]);
			server.run();
		}
		catch(const std::exception& e)
		{
			std::cerr << "Exception caught: " << e.what() << std::endl;
		}
	}
}
