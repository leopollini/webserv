/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receive_request.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/18 18:59:23 by fedmarti          #+#    #+#             */
/*   Updated: 2024/09/19 21:39:03 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Responser.hpp"
#include "Webserv.hpp"

//sets method type, url and protocol version (GET, /, HTTP/1.1)
static void parse_request_line(request_t &request, const string &req_line)
{	
	size_t	field_begin;
	if (!req_line.compare(0, 4, "GET "))
	{
		request.type = GET;
		field_begin = 4;
	}
	else if (!req_line.compare(0, 5, "POST "))
	{
		request.type = POST;
		field_begin = 5;
	}
	else if (!req_line.compare(0, 7, "DELETE "))
	{
		request.type = DELETE;
		field_begin = 7;
	}
	else if (!req_line.compare(0, 5, "HEAD "))
	{
		request.type = HEAD;
		field_begin = 5;
	}
	else
	{
		request.type = INVALID;
		return ;
	}

	field_begin = req_line.find_first_not_of(' ', field_begin);
	if (field_begin == string::npos)
	{
		request.type = INVALID;
		return ;
	}

	size_t	field_end = req_line.find(' ', field_begin);
	size_t	protocol_version = req_line.find("HTTP/");
	if (field_end == string::npos || protocol_version == string::npos)
	{
		if (field_end != string::npos)
		{
			request.type = INVALID;
			return ;
		}
		string(DEFAULT_PROTOCOL).copy(request.http_version, 3, 0);
		request.uri = req_line.substr(field_begin);
		return ;
	}
	request.uri = req_line.substr(field_begin, field_end - field_begin);
	
	req_line.copy(request.http_version, 3, protocol_version + 5);
	//checks if http_version matches "1.n" or "1"
	if (request.http_version[0] != '1' ||
	(request.http_version[1] != '\0' && (request.http_version[1] != '.' || !isdigit(request.http_version[2]))))
		request.type = INVALID;
}	

static void parse_header_line(string &line, std::map<string, string> &map, char separator_char = ':')
{
	string key, value;
	size_t	separator = line.find(separator_char);
	if (separator == string::npos)
		return ;

	key = strip(line.substr(0, separator), " \n\r");
	value = strip(line.substr(separator + 1, line.length() - separator), " \n\r");
	map[key] = value;
}

static void parse_request_header(BetterSocket &socket, request_t &request)
{
	//parses header into request variable map (request.header)
	std::map<string, string> &map = request.header;
	
	map.clear();

	string line = socket.getLine();
	while (line != "" && line != CRNL && socket.wasReadSuccessful())
	{
		parse_header_line(line, map);
		line = socket.getLine();
	}	
}

//reads n bytes from the given fd to complete a previously incomplete request
static	req_t continue_incomplete_request(request_t &request, int fd)
{
	size_t	tot_size = static_cast<size_t>(atoi(request.header[H_BODY_SIZE].c_str()));
	string	body_tail = "";
	int		bytes_read = 1;
	char	*read_buffer;
	
	bytes_read = Webserv::socketRead(fd, &read_buffer, (tot_size - request.body.size() + 1));
	if (bytes_read < 0)
		return (INVALID);
	read_buffer[bytes_read] = 0;
	body_tail += read_buffer;
	request.body += body_tail;
	if (tot_size > request.body.size())
		return (INCOMPLETE);

	request.complete = true;
	printHttpRequest(request);
	return (request.type);
}

static inline req_t chunked_read(BetterSocket &socket, request_t &request)
{
	string line = socket.getChunk();

	while (line != "")
	{
		request.body += line;
		line = socket.getChunk();
	}
	if (!socket.wasReadSuccessful())
		request.type = INCOMPLETE; 
	return (request.type);
}

req_t Server::_receiveBody(request_t &request) throw (Server::BodyMsgTooLong)
{			

	bool chunked_encoding = transfer_encoding::is_encoded(request);

	size_t body_size = static_cast<size_t>(atoi(request.header[H_BODY_SIZE].c_str()));
	string &max_body_size = getEnv(L_MAX_BODY_SIZE, request.loc);

	if (max_body_size != "" && body_size > (size_t)atoi(max_body_size.c_str()))
		throw Server::BodyMsgTooLong();
	
	if (chunked_encoding)
	{
		chunked_read(_sock, request);
		return (request.type);
	}

	request.body = _sock.getBytes(body_size);
	if (request.body.size() < body_size)
		request.type = INCOMPLETE;
	return (request.type);
}

//	################ Here are other function definitions of classes that depend to Server / to which Server is dependant. Sorry i could not create a separate file ):
void	request_t::littel_parse(Server *s)
{
	if (loc)
		uri = uri.substr(loc->dir.size());
	else
		cout << "LOCATION NOT FOUND!\n";
	if (uri[0] != '/')
		uri = '/' + uri;
	uri = s->getEnv(LOC_ROOT, loc) + uri;
	while (*--uri.end() == '/')	// erases trailing '/'s if present
  		uri.erase(--uri.end());
	std::cout << "requested dir be: \'" << uri << "\'\n";
}

req_t Server::receive(int fd)
{
	request_t	&request = _current_request;
	string		req_line;
	string		req_header;
	size_t		bytes_read;
	char		*read_buffer;
	cout << "Readimg from " << fd << "...\n";
	
	if (_sock.sockRead() < 1)
		return (INVALID);
	
	// if (!request.complete)
		// return (continue_incomplete_request(request, fd)); //can also return incomplete
	
	request.header.clear();
	string req_line = _sock.getLine();

	if (req_line == "" || !_sock.wasReadSuccessful())
		return (INVALID);

	HttpRequestLog(req_line, fd);
	parse_request_line(request, req_line);

	parse_request_header(_sock, request);
	if (!_sock.wasReadSuccessful())
		throw HeadMsgTooLong();

	if (request.type == INVALID)
		return (INVALID);

	// truncate location identification part of dir
	matchRequestLocation(request);
	request.littel_parse(this); //wtf does this even do

	if (request.type == POST || request.type == DELETE) // all handled methods with body
		_receiveBody(request);
	if (request.type != INCOMPLETE)
		printHttpRequest(request);
	// return parseMsg(fd);
	return request.type;
}

