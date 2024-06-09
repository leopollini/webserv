/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 13:29:09 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/09 21:15:43 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/utils.hpp"

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
