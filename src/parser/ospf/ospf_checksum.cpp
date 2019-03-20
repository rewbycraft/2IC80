//
// Created by kaj on 1-3-19.
//

#include <vector>
#include "ospf_checksum.h"


std::uint16_t parser::checksum::ospf::calcChecksum(uint128_t sourceAddress, uint128_t destAddress,
		std::uint32_t length, parser::bytevector data)
{
	// Initialize hash.
	std::uint16_t hash = 0;

	// Add source address.
	auto *p = (std::uint8_t *) &sourceAddress;
	for (int i = 0; i < sizeof(uint128_t) / sizeof(std::uint8_t); i++) {
		hash += p[i];
	}

	// Add destination address.
	p = (uint8_t *) &destAddress;
	for (std::uint8_t i = 0; i < sizeof(uint128_t) / sizeof(std::uint8_t); i++) {
		hash += p[i];
	}

	// Add length value.
	p = (uint8_t *) &length;
	for (std::uint8_t i = 0; i < sizeof(std::uint32_t) / sizeof(std::uint8_t); i++) {
		hash += p[i];
	}

	for (std::size_t i = 0; i < data.size(); i++) {
		if (i == 13 || i == 14) continue;
		hash += data[i];
	}

	return hash;
}

bool parser::checksum::ospf::verifyChecksum(uint128_t sourceAddress, uint128_t destAddress,
		std::uint32_t length, parser::bytevector data)
{
	return (parser::checksum::ospf::calcChecksum(sourceAddress, destAddress, length, data) == 0);
	/*
	// Initialize hash.
	std::uint16_t hash = 0;

	// Add source address.
	auto *p = (std::uint8_t *) &sourceAddress;
	for (int i = 0; i < sizeof(uint128_t) / sizeof(std::uint8_t); i++) {
		hash += p[i];
	}

	// Add destination address.
	p = (uint8_t *) &destAddress;
	for (std::uint8_t i = 0; i < sizeof(uint128_t) / sizeof(std::uint8_t); i++) {
		hash += p[i];
	}

	// Add length value.
	p = (uint8_t *) &length;
	for (std::uint8_t i = 0; i < sizeof(std::uint32_t) / sizeof(std::uint8_t); i++) {
		hash += p[i];
	}

	// Add next header field.
	hash += 89;

	// Add data fields.
	for (std::uint8_t i = 0; i < data.size(); i++) {
		for (std::uint8_t j = 0; j < data[i].second; j++) {
			if (i == 13 || i == 14) continue;
			hash += header[i].first[j];
		}
	}

	return hash == 0;*/
}