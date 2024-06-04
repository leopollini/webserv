	/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*    Parser.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <fedmarti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 15:52:31 by fedmarti          #+#    #+#             */
/*   Updated: 2024/06/04 16:21:48 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <parsing_util.hpp>
#include <fstream>

using std::distance;

using namespace Parsing;

using std::list;

typedef string::iterator strIt;


string read_file(string filename) throw (BadFile)
{
	std::ifstream file(filename);
	
	if (file.fail())
		throw (BadFile());

	string line = "";
	string content = "";
	while (getline(file, line))
	{
		content += line;
	}
	file.close();
	return (content);
}

static token new_token(string &string, size_t start, size_t &end) throw (ErrorType)
{
	token new_t = {"", Basic};

	if (string[start] == '{' || string[start] == '}' || string[start] == ';' || string[start] == '=')
	{
		end = start + 1;
		new_t.type = static_cast<enum type>(string[start]);
	}
	new_t.content = string.substr(start, end - start);
	if (new_t.content[0] == '"')
	{
		new_t.content = new_t.content.substr(1, new_t.content.size() - 2);
		new_t.type = StringLiteral;
	}
	return (new_t);
}


list<token> tokenize (string content) throw (Error)
{
	list<token> lst;

	for (size_t i = 0; i < content.size(); i++)
	{
		i = content.find_first_not_of(WHITESPACE, i);

		token	new_t;
		size_t	end = i;
		
		if (content[i] == '"' || content[i] == '\'')
		{
			end = content.find(content[i], i + 1);
			if (end == string::npos)
				throw (UnclosedQuote());
		}
		else
		{
			end = content.find_first_of(BREAK_CHAR, end);
			if ( end == string::npos )
				end = content.size();
		}
		
		if (end - i > 1) // may have to change
			new_t = new_token(content, i, end);
		lst.push_back( new_t );
		i = end;
	}
	return (lst);
}


/*
{
	server{}
}
*/