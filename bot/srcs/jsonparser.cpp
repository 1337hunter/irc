/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jsonparser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/11 14:09:18 by salec             #+#    #+#             */
/*   Updated: 2021/01/17 18:19:02 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "jsonparser.hpp"

typedef std::string::const_iterator			str_it;
#define JSON_PARSE_EXCEPTION	std::invalid_argument("Invalid json format")
#define JSON_KEY_EXCEPTION		std::invalid_argument("Invalid json key")
#define JSON_VAL_EXCEPTION		std::invalid_argument("Invalid json value")

enum	jtype
{
	jnull,
	jbool,
	jnum,
	jstr,
	jarr,
	jobj,
	invalid
};

void		skipspace(str_it *it, str_it const end)
{
	while (*it != end && std::isspace(**it))
		(*it)++;
}

jtype		getjtype(str_it it, str_it const end)
{
	skipspace(&it, end);

	if (it == end)
		return (invalid);
	if (*it == '"')
		return (jstr);
	if (*it == '[')
		return (jarr);
	if (*it == '{')
		return (jobj);

	std::string	tmp = std::string(it, end);

	if (tmp.find_first_of("-0123456789") == 0)
		return (jnum);
	if (tmp.size() >= 4 && tmp.substr(0, 4) == "null")
		return (jnull);
	if ((tmp.size() == 5 && tmp.substr(0, 5) == "false") ||
		(tmp.size() == 4 && tmp.substr(0, 4) == "true"))
		return (jbool);

	return (invalid);
}

std::string	getnextjsonstr(str_it *it, str_it const end, bool key = false)
{
	std::string	res;

	jtype		type = getjtype(*it, end);
	if (type == invalid)
		throw (JSON_PARSE_EXCEPTION);
	if (key && type != jstr)
		throw (JSON_KEY_EXCEPTION);

	str_it	tmp = *it;
	if (type == jstr)
	{
		if (*tmp != '"')
			throw (JSON_PARSE_EXCEPTION);
		res += '"';
		tmp++;
		for ( ; tmp != end && (*tmp != '"' ||
			(res.size() > 0 && res[res.size() - 1] == '\\')); tmp++)
			res += *tmp;
		if (tmp == end || *tmp != '"')
			throw (JSON_PARSE_EXCEPTION);
		tmp++;
		res += '"';
	}
	else if (type == jnum)
	{
		size_t	numend = std::string(tmp, end).find_first_not_of("0123456789eE.+-");
		if (numend == 0 || numend == std::string::npos)
			throw (JSON_PARSE_EXCEPTION);
		res += std::string(tmp, tmp + numend);
		tmp += numend;
	}
	else if (type == jbool || type == jnull)
	{
		std::string	tmpstr = std::string(tmp, end);
		if (tmpstr.find("true") == 0 || tmpstr.find("null") == 0)
		{
			res = tmpstr.substr(0, 4);
			tmp += 4;
		}
		else if (tmpstr.find("false") == 0)
		{
			res = tmpstr.substr(0, 5);
			tmp += 5;
		}
	}
	else if (type == jarr)
	{
		if (*tmp != '[')
			throw (JSON_PARSE_EXCEPTION);
		res += '[';
		tmp++;
		skipspace(&tmp, end);

		while (tmp != end && *tmp != ']')
		{
			std::string	arrval = getnextjsonstr(&tmp, end);
			res += arrval;
			skipspace(&tmp, end);
			if (tmp == end || (*tmp != ',' && *tmp != ']'))
				throw (JSON_PARSE_EXCEPTION);
			if (*tmp == ',')
			{
				res += ',';
				tmp++;
			}
		}
		if (tmp == end || *tmp != ']')
			throw (JSON_PARSE_EXCEPTION);
		res += ']';
		tmp++;
	}
	else if (type == jobj)
	{
		if (*tmp != '{')
			throw (JSON_PARSE_EXCEPTION);
		res += '{';
		tmp++;
		skipspace(&tmp, end);

		while (tmp != end && *tmp != '}')
		{
			std::string	key, val;
			// getting key
			key = getnextjsonstr(&tmp, end, true);
			skipspace(&tmp, end);

			// getting value
			val = getnextjsonstr(&tmp, end);
			skipspace(&tmp, end);

			// finishing key-value pair
			if (*tmp != ',' && *tmp != '}')
				throw (JSON_PARSE_EXCEPTION);
			res += key + ':' + val;
			if (*tmp == ',')
			{
				tmp++;
				res += ',';
			}
		}
		if (tmp == end || *tmp != '}')
			throw (JSON_PARSE_EXCEPTION);
		res += '}';
		tmp++;
	}

	*it = tmp;

	// moving on to value if it was a key
	if (key)
	{
		if (res.size() <= 2 || res == "\"\"")
			throw (JSON_KEY_EXCEPTION);
		if (*it == end || **it != ':')
			throw (JSON_PARSE_EXCEPTION);
		(*it)++;
	}
	return (res);
}

void		processjsonstr(std::string &str)
{
	str = str.substr(1, str.size() - 2);

	size_t	found = 0;
	while ((found = str.find("\\\"")) != std::string::npos)
		str = str.substr(0, found) + str.substr(found + 2);
}

jsonarray	jsonparse(std::string const &input)
{
	str_it const end = input.end();
	str_it it = input.begin();

	skipspace(&it, end);
	if (it == end || *it != '{')
		throw (JSON_PARSE_EXCEPTION);
	it++;

	skipspace(&it, end);
	if (it == end)
		throw (JSON_PARSE_EXCEPTION);

	jsonarray	res;
	while (it != end && *it != '}')
	{
		keyval	entry;
		// getting key
		entry.first = getnextjsonstr(&it, end, true);
		processjsonstr(entry.first);
		skipspace(&it, end);

		// getting value
		entry.second = getnextjsonstr(&it, end);
		if (entry.second[0] == '"')
			processjsonstr(entry.second);
		skipspace(&it, end);

		// finishing key-value pair
		if (*it != ',' && *it != '}')
			throw (JSON_PARSE_EXCEPTION);
		if (*it == ',')
			it++;
		res.insert(entry);
		skipspace(&it, end);
	}
	if (it == end || *it != '}')
		throw (JSON_PARSE_EXCEPTION);
	return (res);
}
