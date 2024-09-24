/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 13:07:36 by fedmarti          #+#    #+#             */
/*   Updated: 2024/09/24 15:33:19 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utils.hpp>

using std::distance;

using namespace Parsing;

using std::list;

typedef string::iterator strIt;

// any implementation MUST handle exception!!!
string Parsing::read_file(string filename) throw (BadFile)
{
	// if (!C_OK(flags))
	// 	throw (BadFile("file does not exist"));
	// if (!(C_READ & flags))
	// 	throw (BadFile("missing read permissions"));
	// if (C_DIR & flags)
	// 	throw (BadFile("is a directory"));

	std::ifstream file(filename.c_str());

	if (!file)
	{
		file.close();
		throw (BadFile(filename));
	}

	std::ostringstream ss;
	ss << file.rdbuf();
	
	// std::cout << content;
	file.close();
	return (ss.str());
}

//assigns the line and column variables by iterating over the string up to the given position
//returns and doesn't assign if pos is either out of bounds or equal to npos
bool	Parsing::line_and_col(string &str, size_t pos, ushort &line, ushort &col)
{
	if (pos == string::npos || pos >= str.size())
		return (false);

	size_t start_line = str.find_last_of('\n', pos);
	if (start_line == string::npos)
		start_line = 0;
	line = std::count(str.begin(), str.begin() + pos, '\n') + 1;
	col = static_cast<ushort>(pos - start_line);
	return (true);
}

static token new_token(string &string, size_t start, size_t &end) throw (ErrorType)
{
	token new_t;

	if (string[start] == '{' || string[start] == '}' || string[start] == ';' || string[start] == '=')
	{
		end = start + 1;
		new_t.type = static_cast<enum type>(string[start]);
	}
	new_t.content = string.substr(start, end - start);
	if (new_t.content[0] == '"' || new_t.content[0] == '\'')
	{
		new_t.content = new_t.content.substr(1, new_t.content.size() - 2);
		new_t.type = STRINGLITERAL;
	}
	if (new_t.content[0] == '#')
		new_t.type = COMMENT;
	line_and_col(string, start, new_t.line_n, new_t.char_count);
	return (new_t);
}


// #include <iostream>
// #include <algorithm>
//Will be removed
// const std::map<enum type, const char *> E_NAME = {{BASIC, "Basic"},
// 		{STRINGLITERAL, "StringLiteral"},
// 		{ASSIGNMENT , "Assignment"},
// 		{OPENBLOCK, "OpenBlock"},
// 		{CLOSEBLOCK, "CloseBlock"},
// 		{BREAKLINE, "BreakLine"},
// 		{COMMENT, "Comment"}	
// 	};

static void _print_token(token tkn)
{
	// std::cout << "content: " << tkn.content << " type: " << E_NAME.at(tkn.type) << " starts at line " << tkn.line_n << " col " << tkn.char_count << std::endl;
	if (tkn.type == OPENBLOCK)
		std::cout << std::endl;
	std::cout << tkn.content;
	if (tkn.type == BREAKLINE || tkn.type == CLOSEBLOCK || tkn.type == COMMENT || tkn.type == OPENBLOCK)
		std::cout << std::endl;
	else
		std::cout << " ";
}

void	Parsing::print_tokens(list<token> lst)
{
	std::for_each(lst.begin(), lst.end(), _print_token);
}

void	error_position(string &str, size_t pos)
{
	ushort line, col;
	if (!line_and_col(str, pos, line, col))
		return ;
	std::cerr << "error at line " << line << " char n " << col << ": ";
}

list<token> Parsing::tokenize (string content) throw (Error)
{
	list<token> lst;
	short	open_bracket = 0;

	for (size_t i = 0, end; i < content.size() && i != string::npos; i = content.find_first_not_of(WHITESPACE, end))
	{
		token	new_t;
		
		//string literal case
		if (content[i] == '"' || content[i] == '\'')
		{
			end = content.find(content[i], i + 1);
			if (end == string::npos)
				throw (UnclosedQuote(content, i));
				
			end++;
		}
		else if (content[i] == '#') // comment case
			end = content.find('\n', i);
		else //default
			end = content.find_first_of(BREAK_CHAR, i + 1);

		open_bracket += (content[i] == '{') - (content[i] == '}');
		if (open_bracket < 0) //checks for too many closed brackets
			throw (MismatchedBrackets(content, i));

		if (end == string::npos) //eof reached
			end = content.size();
		if (end == i)
			break ;
		new_t = new_token(content, i, end);

		if (new_t.type == COMMENT) //skips comments
			continue ;

		lst.push_back( new_t );
	}
	
	if (open_bracket)
		throw (MismatchedBrackets(open_bracket));
	return (lst);
}



//server { path = /dir ; } 
/*
{
	server{ }
}
*/