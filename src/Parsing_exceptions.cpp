/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing_exceptions.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <fedmarti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 17:14:53 by fedmarti          #+#    #+#             */
/*   Updated: 2024/09/25 18:24:24 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utils.hpp>
using namespace Parsing;

BadFile::BadFile( void ) throw() : _err_str("Bad file")
{
	;
}

BadFile::BadFile(string str) throw() : _err_str("Bad file: " + str)
{
	;
}

BadFile::~BadFile() throw()
{
	;
}

UnclosedQuote::UnclosedQuote() throw() : _err_str("Unclosed Quote")
{
	;
}
UnclosedQuote::UnclosedQuote( string &str, size_t pos ) throw() : _err_str("Unclosed Quote")
{
	ushort line, col;

	if (line_and_col(str, pos, line, col))
		_err_str += " at line" + itoa(static_cast<int>(line)) + ", char " + itoa(static_cast<int>(col));
}

UnclosedQuote::~UnclosedQuote() throw()
{
	;	
}

MismatchedBrackets::MismatchedBrackets( ) : _err_str("Mismatched Brackets")
{
	;
}
MismatchedBrackets::MismatchedBrackets( string &str, size_t pos ) : _err_str("Mismatched Brackets")
{
	ushort line, col;

	if (line_and_col(str, pos, line, col))
		_err_str += " at line " + itoa(static_cast<int>(line)) + ", char " + itoa(static_cast<int>(col));
}

MismatchedBrackets::MismatchedBrackets ( int unclosed_n )
{
	_err_str = itoa(unclosed_n) +  " Unclosed brackets at eof";
}
		
MismatchedBrackets::~MismatchedBrackets() throw()
{
	;
}

MissingSemicolon::MissingSemicolon( ) : _err_str("Missing Semicolon")
{
	;
}

MissingSemicolon::MissingSemicolon( string &str, size_t pos ) : _err_str("Missing Semicolon")
{
	ushort line, col;

	if (line_and_col(str, pos, line, col))
		_err_str += " at line " + itoa(static_cast<int>(line)) + ", char " + itoa(static_cast<int>(col));;
}

MissingSemicolon::~MissingSemicolon() throw()
{
	;
}


