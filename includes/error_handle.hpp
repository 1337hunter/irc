#ifndef ERROR_HANDLE_HPP
# define ERROR_HANDLE_HPP

# include <iostream>
# define SELECT 0

void	usage_exit(std::string const &exename);
void	error_exit(std::string const &s1);
void	error_exit(std::string const &s1, std::string const &line,
	size_t line_number);
void	error_exit(int code);

// put a std::string pointer to u if needed
int		SSLErrorCallback(const char *str, size_t len, void *u);

#endif
