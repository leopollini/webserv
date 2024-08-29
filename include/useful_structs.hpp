/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   useful_structs.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 16:58:53 by lpollini          #+#    #+#             */
/*   Updated: 2024/08/29 19:25:53 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USEFUL_STRUCTS_HPP
# define USEFUL_STRUCTS_HPP

# include "utils.hpp"

struct	location_t
{
	conf_t	stuff;
	string	dir;
	char	allows;
	str_set	allowed_extensions; // ???
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
