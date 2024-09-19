/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   transfer_encoding.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 21:11:48 by fedmarti          #+#    #+#             */
/*   Updated: 2024/09/12 21:29:49 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "utils.hpp"

namespace transfer_encoding {
	typedef enum e_encoding_types {
		Chunked,
		Err,
		Void
	} type;

	class UnsupportedEncoding : public std::exception
	{
		virtual const char *what() const throw() {return "Unsupported transfer encoding type";}
	};
	bool is_encoded(request_t &request) throw (UnsupportedEncoding);
	void decode_request_body(request_t &request) throw (UnsupportedEncoding);
	void encode_body(request_t &request, type encoding) throw (UnsupportedEncoding);

};

