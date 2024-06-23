/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   useful_structs.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 16:58:53 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/22 18:27:35 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USEFUL_STRUCTS_HPP
# define USEFUL_STRUCTS_HPP

# include "utils.hpp"

typedef std::map<string, string> var_map_t;

struct	location_t
{
	conf_t	stuff;
	string	dir;
	char	allows;
	str_set	allowed_extensions;
};

struct request_t
{
	req_t		type;
	string		dir;
	var_map_t	header;
	char		http_version[4];
	string		body;
	string		root;
	location_t	*loc;

	void	littel_parse(Server *s);
	request_t() : type(INVALID), dir(""), header(), http_version(DEFAULT_PROTOCOL), body(""), root(""), loc(NULL) {};
};

#endif
