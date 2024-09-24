/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   useful_structs.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 16:58:53 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/24 11:13:28 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USEFUL_STRUCTS_HPP
# define USEFUL_STRUCTS_HPP

# include "utils.hpp"

typedef std::map<string, string> var_map_t;

struct	location_t
{
	conf_t		stuff;
	string		dir;
	char		allows;
	// stores all valid extensions as strings: ie {".txt", ".cpp" ...}
	str_set_t	allowed_extensions;

	location_t() : dir(""), allows(-1), allowed_extensions() {}
};

struct request_t
{
	req_t		type;
	string		uri;
	var_map_t	header;
	char		http_version[4];
	string		body;
	string		root;
	location_t	*loc;
	bool		complete;

	void	littel_parse(Server *s);
	request_t() : type(INVALID), uri(""), header(), http_version(DEFAULT_PROTOCOL), body(""), root(""), loc(NULL), complete(true) {};
};

struct __return_info
{
	int		code;
	string	dir;
};

#endif
