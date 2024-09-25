/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_util.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 15:44:11 by fedmarti          #+#    #+#             */
/*   Updated: 2024/09/25 12:53:45 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <list>
#include <string>
#include <exception>
#include <map>

#define WHITESPACE "\r\f\v\n\t "
#define BREAK_CHAR "\r\f\v\n\t {};\"'#" 
using std::string;
using std::list;

namespace Parsing
{
	enum type {
		BASIC,
		STRINGLITERAL = '\"',
		ASSIGNMENT = '=',
		OPENBLOCK = '{',
		CLOSEBLOCK = '}',
		BREAKLINE = ';',
		COMMENT = '#'
	};

	struct token
	{
		string content;
		enum type type;
		ushort line_n;
		ushort char_count;

		token &operator = ( const token & rhs )
		{
			content = rhs.content;
			type = rhs.type;
			line_n = rhs.line_n;
			char_count = rhs.char_count;
			return (*this);
		};
		token(void) : content(""), type(BASIC), line_n(0), char_count(0) {};
		token(string c, enum type t, ushort line, ushort col) : content(c), type(t), line_n(line), char_count(col) {};
		token( const token & ref ) : content(ref.content), type(ref.type), line_n(ref.line_n), char_count(ref.char_count) {};
		~token(){};
	};

	bool		line_and_col(string &str, size_t pos, ushort &line, ushort &col);


	class Error : public std::exception
	{
		public:
		virtual const char *what() const throw()
		{
			return ("Parsing Error");
		}
		virtual ~Error() throw () {};
	};
	class BadFile : public Error
	{
		string _err_str;
		public:
		BadFile() throw();
		BadFile(string) throw();
		~BadFile() throw();
		const char *what() const throw()
		{
			return (_err_str.c_str());
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
			return (_err_str.c_str());
		};
	public:
		UnclosedQuote() throw();
		UnclosedQuote( string &str, size_t pos ) throw();
		~UnclosedQuote() throw();
	private:
		string _err_str;
	};

	class MismatchedBrackets : public Error
	{
		const char *what() const throw()
		{
			return (_err_str.c_str());
		};
	public:
		MismatchedBrackets( );
		MismatchedBrackets( string &str, size_t pos );
		MismatchedBrackets ( int unclosed_n );
		~MismatchedBrackets() throw();
	private:
		string _err_str;
	};
	
	list<token>	tokenize (string content) throw (Error);
	string 		read_file (string filename) throw (BadFile);

	void	print_tokens(list<token> lst);  // for debugging
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