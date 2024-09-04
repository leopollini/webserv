/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TransferDecoder.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 20:01:16 by fedmarti          #+#    #+#             */
/*   Updated: 2024/09/03 20:38:03 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

class	TransferDecoder {
private:
	static void _chunkedDecoding(request_t &request);
public:
	
	class UnsupportedEncoding : public std::exception
	{
		virtual const char *what() const throw() {return "Unsupported transfer encoding type";}
	};

	
	static bool isEncoded(request_t &request) throw (UnsupportedEncoding);
	static void decodeRequestBody(request_t &request) throw (UnsupportedEncoding);
};
