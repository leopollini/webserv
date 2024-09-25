/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BetterEnv.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <fedmarti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 23:26:31 by fedmarti          #+#    #+#             */
/*   Updated: 2024/09/26 00:07:27 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

	size_t 		size() const {return (_size);};
	const char	**c_envp() {return (_env);};
	void		addVariable(string key_plus_value);
	void		addVariable(string key, string value);
	void		removeVariable(string key);
};
