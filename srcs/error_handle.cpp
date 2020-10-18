#include "error_handle.hpp"

void	error_exit(std::string s1)
{
	std::cerr << s1;
	std::cerr << std::endl;
	exit(1);
}
