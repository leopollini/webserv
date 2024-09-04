/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TransferEncoder.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 22:11:31 by fedmarti          #+#    #+#             */
/*   Updated: 2024/09/03 22:26:47 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

class TransferEncoder {
private:
	static const string _TYPES[];
public:

typedef enum e_encoding_types {
		Chunked,
		Err,
		Void
	} type;

	static void encodeBody(request_t &request, type encoding);
};


