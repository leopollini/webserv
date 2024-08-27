/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:39 by lpollini          #+#    #+#             */
/*   Updated: 2024/08/27 15:36:07 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/Webserv.hpp"

int main(int argn, char *argv[], char *envp[])
{
	Webserv	&serv = Webserv::getInstance();

	(void)argn;
	(void)envp;
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
