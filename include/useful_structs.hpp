/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   useful_structs.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 16:58:53 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/04 15:12:03 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USEFUL_STRUCTS_HPP
# define USEFUL_STRUCTS_HPP

# include "utils.hpp"

struct	location_t
{
	conf_t		stuff;
	string		dir;
	char		allows;
	// stores all valid extensions as strings: ie {".txt", ".cpp" ...}
	str_set_t	allowed_extensions;
};

struct request_t
{
	req_t		type;
	string		dir;
	string		host;
	string		root;
	location_t	*loc;

	void	littel_parse(Server *s);
};

struct __return_info
{
	int		code;
	string	dir;
};

#endif
