/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   transfer_encoding.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 22:17:36 by fedmarti          #+#    #+#             */
/*   Updated: 2024/09/26 15:02:48 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "transfer_encoding.hpp"
#include <sstream>

static void chunked_encoding(request_t &r)
{
	//i probably gotta change this
	static_cast<void>(r);
}

void transfer_encoding::encode_body(request_t &request, type encoding) throw (UnsupportedEncoding)
{
	if (encoding == Err)
		throw (UnsupportedEncoding());
	if (encoding == Void)
		return ;
	if (encoding == Chunked)
	{
		chunked_encoding(request);
		return ;
	}

}

bool transfer_encoding::is_encoded( request_t &request ) throw (UnsupportedEncoding)
{
	string	encoding_type = request.header[H_TRANSFER_ENCODING];

	if (encoding_type == "")
		return (false);

	if (encoding_type == "chunked")
		return (true);

	throw (UnsupportedEncoding());
}

static inline size_t _next(size_t i, string str)
{
	size_t pos = str.find(CRNL, i);

	if (pos == string::npos)
		return (str.size());
	return (pos);
} 

static void chunked_decoding(request_t &request)
{
	string new_body = "";
	string &old_body = request.body;


	for (size_t i = 0; i < old_body.size(); )
	{
		size_t endl = _next(i, old_body);
		string hex = old_body.substr(i, i - endl);
		std::stringstream strstr;

		size_t chunk_size;
		
		strstr << std::hex << hex;
		strstr >> chunk_size;
		
		if (chunk_size == 0)
			break ;
		i = endl + 2;
		endl = _next(i, old_body);
		new_body += old_body.substr(i, chunk_size);
		i = endl + 2;
	}
}

void transfer_encoding::decode_request_body(request_t &request) throw (UnsupportedEncoding)
{
	string	encoding_type = request.header[H_TRANSFER_ENCODING];

	if (encoding_type == "")
		return ;

	if (encoding_type == "chunked")
	{
		chunked_decoding(request);	
		return ;
	}
	throw (UnsupportedEncoding());
}