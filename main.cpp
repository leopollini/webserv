/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:39 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/02 16:59:30 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/Webserv.hpp"

int main(int argn, char *argv[], char *envp[])
{
	Webserv	serv(argv[1]);

	try
	{
		serv.parseConfig();
		serv.start();
	}
	catch(const std::exception& e)
	{
		timestamp(e.what(), ERROR);
		cout << '\n';
	}


	timestamp("Beginning static destruction of everything\n", DONE);
	return 0;
}
