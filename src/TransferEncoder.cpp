/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TransferEncoder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 22:17:36 by fedmarti          #+#    #+#             */
/*   Updated: 2024/09/04 00:02:12 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "transfer_encoding.hpp"
#include "TransferEncoder.hpp"

void TransferEncoder::encodeBody(request_t &request, type encoding)
{
	if (encoding == Err)
		throw ();
	if (encoding == Void)
		return ;
	if (encoding == Chunked)

}

const string TransferEncoder::_TYPES[TransferEncoder::Chunked] = "Chunked";
const string TransferEncoder::_TYPES[TransferEncoder::Err] = "Err";
const string TransferEncoder::_TYPES[TransferEncoder::Void] = "Void";