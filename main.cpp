/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2024/09/24 10:58:11 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */

#include "include/Webserv.hpp"

int main(int argn, char *argv[], char *envp[])
{
	Webserv	&serv = Webserv::getInstance();

	//chdir("..");
	(void)argn;
	if (argv[1])
		serv.setConf(argv[1]);

	try
	{
		serv.parseConfig();
		serv.start(envp);
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
