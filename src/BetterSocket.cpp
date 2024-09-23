/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BetterSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/15 02:44:25 by fedmarti          #+#    #+#             */
/*   Updated: 2024/09/23 18:12:18 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BetterSocket.hpp"
#include "Webserv.hpp"

bool	BetterSocket::isLineComplete()
{
	return (strstr(_read_buff, CRNL) != NULL);
}

//reads size bytes into _read_buff 
//returns the result of the read call
//on failure returns -1, flushes the buffer and possibly shuts down the server
size_t	BetterSocket::sockRead()
{
	// char *read_buff = _read_buff;
	int	bytes_read;
	static int read_count = 0;
	size_t	size;

	size = BUFFER_SIZE - _buffer_len;

	bytes_read = read(fd, _read_buff + _buffer_len, size);
	_buffer_len += bytes_read; 

	if (bytes_read < 0)
	{
		timestamp("Failed read at fd: " + itoa(fd), ERROR);
		if (fcntl(fd, F_GETFD) == -1)
		{
			timestamp(" The socket is closed!\n", ERROR, BOLD, false);
			timestamp(" Shutting down server...\n", WARNING);
			Webserv::getInstance().downServer(fd);
		}
		else
			std::cout << std::endl;
		flushBuffer();
		// read_buff = NULL;
	}
	else
		_read_buff[_buffer_len] = 0;
	if (read_count++ == 4)
		close(fd);
	return (bytes_read);
}

//removes n bytes from the read buffer, without taking in account its contents, can cause overflow
string BetterSocket::_flush_bytes(size_t n)
{
	string content(_read_buff, n);
	
	memmove(_read_buff + n, _read_buff, _buffer_len - n);
	_buffer_len -= n;
	_read_buff[_buffer_len] = '\0';
	return (content);
}

//returns one line from the socket read buffer
//in case CRNL is not found then it's assumed that the line is incomplete
//and the chunk of the line is returned
//BetterSocket::isLineComplete() to check
string	BetterSocket::getLine()
{
	if (_read_buff == "")
		return ("");

	size_t line_size;
	char *line_end = strstr(_read_buff, CRNL);
	if (line_end == NULL)
	{
		return ("");
	}
	
	line_size = line_end - _read_buff + 2;
	string line = _flush_bytes(line_size);
	return (line);
}

//reads n bytes from the socket buffer and returns as many as possible
string	BetterSocket::getBytes(size_t to_read)
{
	string content;
	if (_buffer_len < to_read)
	{
		content = _flush_bytes(_buffer_len);
		_successful_read = false;
		return (content);
	}
	
	content = _flush_bytes(to_read);
	_successful_read = true;
	return (content);
}

//reads from the buffer and flushes everything up to carriage return
void	BetterSocket::flushUntilEnd()
{
	string line;
	do
	{
		line = getLine();
		
	} while (line != "" || line != CRNL);

}

//sets all bytes of the read buffer to 0, sets _buffer_len to 0 and _incomplete_line to false
void	BetterSocket::flushBuffer()
{
	_buffer_len = 0;
	memset(_read_buff, 0, BUFFER_SIZE + 1);
}

void BetterSocket::init(short port, int address)
{
	_successful_read = true;
	flushBuffer();
	// while (0); // prevent inline-ing
	if (fd >= 0)
		close(fd);
	if ((fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
		throw FailedSocketCreation();
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = address;
	addr.sin_port = htons(port);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		throw FailedSocketBind();
	if (listen(fd, CLIENTS_MAX) < 0)
		throw FailedSocketListen();
}

//AA/r/n
//eawfwefawefawefawef/r/n

string	BetterSocket::getChunk()
{
	size_t chunk_size;
	string size_chunk;
	std::stringstream ss;
	string content;

	
	char *carriage_return = strstr(_read_buff, CRNL); 
	if (carriage_return == NULL)
	{
		_successful_read = false;
		return (content);
	}

	string size_chunk = string(_read_buff, carriage_return - _read_buff);
	ss << std::hex << size_chunk;
	ss >> chunk_size;

	char *chunk_start = carriage_return + 2;
	if (strlen(chunk_start) < chunk_size + 2 || (chunk_start + chunk_size) != CRNL)
	{
		_successful_read = false;
		return (content);
	}
	getLine(); //flushes size chunk
	content += _flush_bytes(chunk_size);
	_flush_bytes(2); //flushes crnl		
	_successful_read = true;
	return (content);
}

int BetterSocket::Accept()
{
	len = 0;
	sock = accept(fd, (struct sockaddr *)&client, &len);
	return sock;
}
	
void	BetterSocket::down()
{
	close (fd);
}