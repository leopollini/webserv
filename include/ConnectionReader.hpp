/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionReader.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 18:36:31 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/24 19:08:48 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include "utils.hpp"
# include "definitions.hpp"
# include <string>
# define BUFFER_SIZE 3000ul

class ConnectionReader {
private:
	size_t	_buffer_len;
	char	_read_buff[BUFFER_SIZE + 1];
	string	_flush_bytes(size_t n);
	bool	_successful_read;

public:
	void	flushBuffer();
	void	flushUntilCRNL();

	string	getLine();
	string	getBytes(size_t to_read);
	string	getChunk();
	bool	wasReadSuccessful() const {return _successful_read;};
	
	size_t	sockRead(int input_fd);
};
