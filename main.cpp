/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <fedmarti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:39 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/07 19:44:24 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/Webserv.hpp"

int main(int argn, char *argv[], char *envp[])
{
	Webserv	&serv = Webserv::getInstance();

	if (argv[1])
		serv.setConf(argv[1]);

	try
	{
		serv.parseConfig();
		serv.start();
	}
	catch (Parsing::Error &e)
	{
		timestamp("Error parsing " + serv.getConf() + ": " + string(e.what()) + "\n", ERROR);
	}
	catch(const std::exception& e)
	{
		timestamp(e.what(), ERROR);
		cout << '\n';
	}


	timestamp("Beginning static destruction of everything\n", DONE);
	return 0;
}
