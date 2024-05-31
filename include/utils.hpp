/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 13:29:37 by lpollini          #+#    #+#             */
/*   Updated: 2024/05/31 17:27:47 by lpollini         ###   ########.fr       */
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
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <poll.h>
# include <csignal>
# include <unistd.h>
# include <list>
# include <map>
# include <deque>
# include "utils.hpp"

# define ERROR RED
# define WARNING YELLOW
# define DEBUG CYAN
# define INFO WHITE
# define DONE GREEN

using std::string;
using std::cout;
typedef std::map<string, string>	conf_t;

typedef	enum	e_colors
{
	GRAYI = 0,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	MAGENTA,
	CYAN,
	WHITE,
	GRAYII,
	GRAYIII,
}				colors;
typedef	enum	e_text
{
	WHITE_O = 0,
	BOLD,
	FADE,
	ITALIC,
	UNDERLINED,
	COLORI,
	COLORII,
	BACKGROUND,
	DISAPPEAR_LOL,
	CROSSED
}				textype;

void	timestamp(string s, colors c = WHITE, textype a = BOLD, bool do_timestamp = true);
string	itoa(int arg);

string	addr_to_str(int addr);

#endif
