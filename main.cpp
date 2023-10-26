#include "utils.hpp"

int	main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "Insert correct arguments!" << std::endl;
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
			std::cerr << e.what() << std::endl;
		}
	}
}
