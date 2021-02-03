#ifndef JSONPARSER_HPP
#define JSONPARSER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <exception>

typedef std::pair<std::string, std::string>	keyval;
typedef std::map<std::string, std::string>	jsonarray;

jsonarray		jsonparse(std::string const &input);

#endif
