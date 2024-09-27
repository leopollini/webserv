/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BetterEnv.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 23:26:31 by fedmarti          #+#    #+#             */
/*   Updated: 2024/09/27 12:31:38 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <vector>

using std::string;
using std::vector;

class BetterEnv {
	char **_env;
	size_t _size;
	size_t	_alloc_size;
public:


	BetterEnv();
	BetterEnv(char **envp);
	~BetterEnv();

	size_t 			size() const; //{return (_size);};
	char * const	*c_envp() {return (_env);};
	void			addVariable(string key_plus_value);
	void			addVariable(string key, string value);
	void			removeVariable(string key);
};
