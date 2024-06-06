/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 13:29:09 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/05 11:42:57 by lpollini         ###   ########.fr       */
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
	cout << "\e[" << c + 30 << ';' << a << 'm';
	if (do_timestamp)
	{
		char date[50];
		tzset();
		time_t now = time(0);
		struct tm tm = *gmtime(&now);
		tm.tm_hour += 2;
		strftime(date, sizeof(date), "[%Y/%m/%d  %H:%M:%S]   ", &tm);
		cout << date;
	}
	cout << s;
	cout << "\e[0m";
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
