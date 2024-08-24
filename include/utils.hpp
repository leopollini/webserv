/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 13:29:37 by lpollini          #+#    #+#             */
/*   Updated: 2024/08/24 17:45:41 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP

# define UTILS_HPP

# include <sstream>
# include <iostream>
# include <string>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <sys/time.h>
# include <csignal>
# include <unistd.h>
# include <list>
# include <map>
# include <deque>
# include <cmath>
# include "utils.hpp"
# include "parsing_util.hpp"
# include <fstream>
# include <algorithm>
# include "definitions.hpp"
# include "useful_structs.hpp"


# define NEW_SERVER "server"
# define NAME "server_name"
# define PORT "listen"
# define LOCATION "location"
# define LOC_ROOT "root"
# define L_DIR "location"
# define L_INDEX "index"
# define L_DIR_LISTING "autoindex"
# define E_405 "e_405"
# define E_404 "e_404"
# define E_403 "e_403"
# define L_AUTOINDEX "autoindex"
# define L_ALLOW_METHODS "allow_methods"
# define LOC_RETURN "return"
# define RETURN_CODE "_return_code"
# define RETURN_DIRECTORY "_return_directory"
# define PERM_REDIR_RET "301"

#endif
