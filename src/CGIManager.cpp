/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIManager.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 13:07:36 by fedmarti          #+#    #+#             */
/*   Updated: 2024/08/29 19:26:23 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/utils.hpp"
#include "../include/CGIManager.hpp"

void	CGIManager::start(const string cgi_path, std::list<string> args)
{
	char const**argv = new char *[args.size() + 2];
	int	t = 1;

	argv[0] = cgi_path.c_str();
	for (std::list<string>::iterator i = args.begin(); i != args.end(); ++i, ++t)
		argv[t] = i->c_str();
	argv[t] = NULL;
	execve(argv[0], (char *const*)argv, _env);
}
