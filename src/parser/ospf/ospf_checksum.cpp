//
// Created by kaj on 1-3-19.
//

#include <vector>
#include "ospf_checksum.h"


std::uint16_t parser::calcOSPFChecksum(std::vector<std::pair<std::uint8_t *, std::uint8_t>> data)
{
	uint16_t hash = 0;
	uint8_t pos = 0;
	for (auto &header : data) {
		for (std::uint8_t i = 0; i < header.second; i++) {
			if (++pos == 13 || pos == 14) continue;
			hash += header.first[i];
		}
	}
	return hash;
}

bool parser::checkOSPFChecksum(uint128_t sourceAddress,
		uint128_t destAddress, std::uint32_t length,
		std::vector<std::pair<std::uint8_t *, std::uint8_t>> data)
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

	// Add next header field.
	hash += 89;

	uint8_t pos = 0;
	for (auto &header : data) {
		for (std::uint8_t i = 0; i < header.second; i++) {
			if (++pos == 13 || pos == 14) continue;
			hash += header.first[i];
		}
	}

	return hash == 0;
}