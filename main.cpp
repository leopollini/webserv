/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:39 by lpollini          #+#    #+#             */
/*   Updated: 2024/05/30 13:58:13 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/Webserv.hpp"

int main(int argn, char *argv[], char *envp[])
{
	Webserv				serv("lolfile");
	// struct BetterSocket	lol;

	try
	{
		// lol.init(8080, INADDR_ANY);
		serv.parseConfig();
		serv.start();
	}
	catch(const std::exception& e)
	{
		timestamp(e.what(), RED);
		std::cout << '\n';
	}


	// pollfd t;
	// t.events = POLLIN | POLLOUT;
	// t.fd = lol.fd;
	// if (poll(&t, 1, 1000) > 0)
	// {
	// 	int a = accept(t.fd, NULL, NULL);
	// 	char b[1000];
	// 	read(a, b, 1000);
	// 	std::cout << b;

	// 	close(a);
	// }
	// close(t.fd);
	timestamp("Beginning static destruction of everything\n", GREEN, BOLD);
	return 0;
}
