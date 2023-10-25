#include "utils.hpp"

int	main(int ac, char **av)
{
	if (ac != 3 && ac != 2)
	{
		std::cerr << "Insert correct arguments!" << std::endl;
		return 1;
	}
	else
	{
		try
		{
			std::string psw;
			if (ac == 3)
				psw = av[2];
			else
				psw = "";
			Server server(av[1], psw);
			server.run();
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
}
