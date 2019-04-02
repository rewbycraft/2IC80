//
// Created by kaj on 1-3-19.
//

#include <vector>
#include "ospf_checksum.h"


std::uint16_t parser::checksum::ospf::calcChecksum(uint128_t sourceAddress, uint128_t destAddress,
		std::uint32_t length, const parser::bytevector& data) {
	// Initialize hash.
	// These values can only overflow if the message is longer than 16 843 009 bytes.
	uint32_t hash0 = 0;
	uint32_t hash1 = 0;

	// Add source address.
	auto *p = (std::uint8_t *) &sourceAddress;
	for (std::size_t i = 0; i < sizeof(uint128_t) / sizeof(std::uint8_t); i+=2) {
		hash0 += p[i];
		hash1 += p[i+1];
	}

	// Add destination address.
	p = (uint8_t *) &destAddress;
	for (std::size_t i = 0; i < sizeof(uint128_t) / sizeof(std::uint8_t); i+=2) {
		hash0 += p[i];
		hash1 += p[i+1];
	}

	// Add length value.
	p = (uint8_t *) &length;
	for (std::size_t i = 0; i < sizeof(std::uint32_t) / sizeof(std::uint8_t); i+=2) {
		hash0 += p[i];
		hash1 += p[i+1];
	}

	// Add next header field.
	hash1 += 89;

	// Add the data to the checksum.
	for (std::size_t i = 0; i < data.size() - 1; i+=2) {
		hash0 += data[i];
		hash1 += data[i+1];
	}

	// Add the last data byte to the checksum if it has an odd number of bytes.
	if (data.size() % 2) {
		hash0 += data[data.size() - 1];
	}

	// Fix the carries.
	for (int i = 0; i < 4; i++) {
		uint32_t h0c = hash0 >> 8;
		hash0 = (hash0 & 0xFF) + (hash1 >> 8);
		hash1 = (hash1 & 0xFF) + h0c;
	}

	return std::uint16_t(0xFFFF) ^ std::uint16_t((hash0 << 8) | hash1);
}

bool parser::checksum::ospf::verifyChecksum(uint128_t sourceAddress, uint128_t destAddress,
		std::uint32_t length, const parser::bytevector& data) {
	return (parser::checksum::ospf::calcChecksum(sourceAddress, destAddress, length, data) == 0);
}