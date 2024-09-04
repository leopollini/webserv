/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TransferDecoder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 20:05:50 by fedmarti          #+#    #+#             */
/*   Updated: 2024/09/03 20:52:01 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TransferDecoder.hpp"
#include <sstream>


bool TransferDecoder::isEncoded( request_t &request ) throw (UnsupportedEncoding)
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

void TransferDecoder::_chunkedDecoding(request_t &request)
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

void TransferDecoder::decodeRequestBody(request_t &request) throw (UnsupportedEncoding)
{
	string	encoding_type = request.header[H_TRANSFER_ENCODING];

	if (encoding_type == "")
		return ;

	if (encoding_type == "chunked")
	{
		_chunkedDecoding(request);	
		return ;
	}
	throw (UnsupportedEncoding());
}