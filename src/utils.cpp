/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <fedmarti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 13:29:09 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/24 20:07:18 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/utils.hpp"
#include "useful_structs.hpp"
string itoa(int arg)
{
	std::ostringstream sstr;
	sstr << std::dec << arg;
	return sstr.str();
}

void	timestamp(string s, colors c, textype a, bool do_timestamp)
{
	std::ostream &out = (c == ERROR)? std::cerr : cout;

	out << "\e[" << c + 30 << ';' << a << 'm';
	if (do_timestamp)
	{
		char date[50];
		tzset();
		time_t now = time(0);
		struct tm tm = *gmtime(&now);
		tm.tm_hour += 2;
		strftime(date, sizeof(date), "[%Y/%m/%d  %H:%M:%S]   ", &tm);
		out << date;
	}
	out << s;
	out << "\e[0m";
}

string addr_to_str(int addr)
{
	std::ostringstream sstr;
	sstr << std::dec << (int)((char *)&addr)[0] << '.';
	sstr << std::dec << (int)((char *)&addr)[1] << '.';
	sstr << std::dec << (int)((char *)&addr)[2] << '.';
	sstr << std::dec << (int)((char *)&addr)[3];
	return sstr.str();
}

timeval	t_delta_time(timeval &b, timeval &a)
{
	timeval res;

	res.tv_sec = a.tv_sec - b.tv_sec + ((a.tv_usec < b.tv_usec) ? 1 : 0);
	res.tv_usec = a.tv_usec < b.tv_usec ? b.tv_usec - a.tv_usec : a.tv_usec - b.tv_usec;
	return res;
}

timeval	timeres(int reset)
{
	static timeval	start;
	timeval			res;

	if (reset)
	{
		gettimeofday(&start, NULL);
		return start;
	}
	gettimeofday(&res, NULL);
	return start.tv_sec ? t_delta_time(start, res) : start;
}

void _print_pool(fd_set &pool, std::string name)
{
    std::cout << "Now printing " << name << "fd pool" << std::endl;
    for ( uint i = 0; i < 128; i++)
        if (FD_ISSET(i, &pool))
            std::cout << i << ' ';
    std::cout << std::endl;
}
// non funziona >:(
char	checkCharacteristics(const char *path)
{
	char c = 0;

	
	if (access(path, F_OK))
		return (c);
	if (!access(path, R_OK))
		c |= C_READ;
	if (!access(path, W_OK))
		c |= C_WRITE;
	if (!access(path, X_OK))
		c |= C_EXEC;
	
	struct stat statbuff;
	
	stat(path, &statbuff);
	if (S_ISREG(statbuff.st_mode))
		c |= C_FILE;
	else if (S_ISDIR(statbuff.st_mode))
		c |= C_DIR;
	return (c);
}
// non funziona >:(
char	checkFastAccess(const char *path)
{	
	if (access(path, R_OK))
		return 0;
	struct stat statbuff;
	
	stat(path, &statbuff);
	if (!S_ISREG(statbuff.st_mode))
		return 0;

	return 1;
}

bool	isOkToSend(char flags)
{
	if (flags & (C_READ | C_FILE))
		return true;
	return false;
}

bool	exists(char flags)
{
	if (flags & (C_FILE | C_DIR))
		return true;
	return false;
}

char	read_allows(string &allow)
{
	char	res = 0;

	if (allow.find("GET") != string::npos)
		res |= F_GET;
	if (allow.find("POST") != string::npos)
		res |= F_POST;
	if (allow.find("DELETE") != string::npos)
		res |= F_DELETE;
	if (allow.find("HEAD") != string::npos)
		res |= F_HEAD;
	if (!res)
		res = ~0;
	return res;
}

//analyzes line and returns the method type
req_t	request_method(string request_line)
{
	size_t	first_not_space = request_line.find_first_not_of(' '), space_pos;
	if (first_not_space == string::npos)
		return (INVALID);
		
	space_pos = request_line.find(' ', first_not_space);
	if (space_pos == string::npos)
		return (INVALID);
	string method = request_line.substr(first_not_space, space_pos);
	
	if (method == "GET")
		return (GET);
	else if (method == "POST")
		return (POST);
	else if (method == "DELETE")
		return (DELETE);
	else if (method == "HEAD")
		return (HEAD);
	
	return (INVALID);
}

string	request_type_str(req_t type)
{
	switch (type)
	{
		case GET:
			return ("GET");
		case POST:
			return ("POST");
		case HEAD:
			return ("HEAD");
		case DELETE:
			return ("DELETE");
		case INVALID:
			return ("INVALID"); 
		default:
			return ("");
	}
	// return ("");
}

string	strip(string str, char *charset)
{
	size_t start, end;

	start = str.find_first_not_of(charset);
	end = str.find_last_not_of(charset);
	if (start == string::npos)
		return ("");
	return (str.substr(start, end - start + 1));
}

void	printHttpRequest(request_t &request, std::ostream &out)
{
	if (request.type == INVALID)
	{
		std::cout << "Invalid request" << std::endl;
		return;
	}

	out << request_type_str(request.type);

	out << " " << request.dir;
	out << " HTTP/" << request.http_version << '\n';
	for (var_map_t::const_iterator it = request.header.begin(); it != request.header.end(); it++)
	{
		out << it->first << ": " << it->second << '\n';
	}
	out << "\r\n\r\n";
	out << request.body << std::endl;
}