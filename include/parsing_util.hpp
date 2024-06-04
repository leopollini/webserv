/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_util.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <fedmarti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 15:44:11 by fedmarti          #+#    #+#             */
/*   Updated: 2024/06/04 18:44:26 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <list>
#include <string>
#include <exception>

#define WHITESPACE "\r\f\v\n\t "
#define BREAK_CHAR "\r\f\v\n\t {};" 
using std::string;

namespace Parsing
{
	enum type {
		Basic,
		StringLiteral,
		Assignment = '=',
		OpenBlock = '{',
		CloseBlock = '}',
		EndLine = ';'
	};

	struct token
	{
		string content;
		enum type type;
	};

	
	class Error : public std::exception
	{
		virtual const char *what() const throw()
		{
			return ("Parsing Error");
		}
	};
	class BadFile : public Error
	{
		const char *what() const throw()
		{
			return ("Bad file");
		}
	};
	class ErrorType : public Error
	{
		const char *what() const throw()
		{
			return ("Error type");
		}
	};
	
	class UnclosedQuote : public Error
	{
		const char *what() const throw()
		{
			return ("Unclosed Quote");
		}
	};
	
	list<token>			tokenize (string content) throw (Error);
	string 				read_file (string filename) throw (BadFile);
};


/*
configuration file must handle:
	port
	host
	default error pages
	limit client body size
	setup routes with rules...
*/


/*
routes rules:
	accepted http methods]
	http redirection
	root
	toggle directory listing
	default file answering root
	cgi based on file extension
	POST/GET
	configuration for uploaded files (by the client)
*/