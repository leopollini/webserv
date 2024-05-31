/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 13:29:09 by lpollini          #+#    #+#             */
/*   Updated: 2024/05/31 17:27:42 by lpollini         ###   ########.fr       */
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
