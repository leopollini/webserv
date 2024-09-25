/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BetterEnv.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <fedmarti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 23:31:16 by fedmarti          #+#    #+#             */
/*   Updated: 2024/09/26 00:17:18 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BetterEnv.hpp"
#include "definitions.hpp"
#include <string.h>

BetterEnv::BetterEnv() : _size(0), _alloc_size(1 + EXTRA_ENV_SPACE)
{
	_env = new char*[_alloc_size];
	_env = NULL;
}

BetterEnv::BetterEnv(char **envp)
{
	size_t size = 0;

	while (envp[size])
		size++;
	_size = size;
	_alloc_size = size + 1 + EXTRA_ENV_SPACE;
	_env = new char*[_alloc_size];

	for (size_t i = 0; i < size; i++)
	{
		size_t len = strlen(envp[i]);
		_env[i] = new char[len + 1];
		
		strncpy(_env[i], envp[i], len);
	}
	_env[size] = NULL;
}

BetterEnv::~BetterEnv()
{
	if (!_env)
		return ;
	for (int i = (int)_size - 1; i >= 0; i--)
	{
		delete _env[i];
	}
	delete [] _env;
}

size_t BetterEnv::size() const
{
	size_t size = 0;
	while (_env[size])
	{
		size++;
	}
	return (size);
}

void BetterEnv::addVariable(string key_plus_value)
{
	if (_size == _alloc_size - 1)
	{
		char **old_arr = _env;
		
		_alloc_size += EXTRA_ENV_SPACE;
		_env = new char*[_alloc_size];
		for (size_t i = 0; i < _size; i++)
			_env[i] = old_arr[i];
		
		delete []old_arr;
	}

	char *new_str = new char[key_plus_value.size() + 1];

	strncpy(new_str, key_plus_value.c_str(), key_plus_value.size() + 1);
	_env[_size++] = new_str;
	_env[_size] = NULL;
}

void BetterEnv::addVariable(string key, string value)
{
	addVariable(key + "=" + value);
}

void BetterEnv::removeVariable(string key)
{
	int index = -1;

	for (size_t i = 0; i < _size; i++)
	{
		if (!strncmp(_env[i], key.c_str(), key.size()) && _env[i][key.size()] == '=')
		{
			index = (int)i;
			break ;
		}
	}
	
	if (index == -1)
		return ;

	delete []_env[index];

	memmove(&_env[index], &_env[index + 1], _size - index);

	_size--;
	_env[NULL];
}
