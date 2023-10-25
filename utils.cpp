#include "utils.hpp"

bool	isNumber(const std::string &str)
{
	for(size_t i = 0; i < str.size(); i++)
	{
		if (!std::isdigit(str[i]))
			return (false);
	}
	return (true);
}

uint16_t	portConverter(const std::string &port)
{
	uint32_t	n = 0;
	size_t		size = port.size();

	for(size_t i = 0; i < size; i++)
	{
		n = (n * 10) + port[i] - 48;
		if (n > std::numeric_limits<uint16_t>::max() || !std::isdigit(port[i]))
			throw(std::invalid_argument("Invalid port"));
	}
	if (n < 1024)
		throw(std::invalid_argument("Invalid port"));
	return (n);
}
