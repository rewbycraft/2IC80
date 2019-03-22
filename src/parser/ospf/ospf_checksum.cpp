//
// Created by kaj on 1-3-19.
//

#include <vector>
#include "ospf_checksum.h"

// tmp
#include <iomanip>
#include <iostream>
#include <bitset>


std::uint16_t parser::checksum::ospf::calcChecksum(uint128_t sourceAddress, uint128_t destAddress,
		std::uint32_t length, const parser::bytevector& data)
{
	// Initialize hash.
	// These values can only overflow if the message is longer than 16 843 009 bytes.
	uint32_t hash0 = 0;
	uint32_t hash1 = 0;

	//std::cout << std::hex << std::uint64_t(sourceAddress) << ":" << std::uint64_t(sourceAddress >> 64) << "\n";
	//std::cout << std::hex << std::uint64_t(destAddress) << ":" << std::uint64_t(destAddress >> 64) << "\n";

	//std::cout << "0x" << std::hex << x << std::dec << " / " << x << " / 0b " << std::bitset<8>(x) << "\n";
	// Add source address.
	auto *p = (std::uint8_t *) &sourceAddress;
	for (std::size_t i = 0; i < sizeof(uint128_t) / sizeof(std::uint8_t); i+=2) {
		hash0 += p[i];
		hash1 += p[i+1];
		//hash += p[i];
		//hash += (p[i] << 8) + (p[i] & 0xFFFF0000);
	}
	std::cout << "\n";


	// Add destination address.
	p = (uint8_t *) &destAddress;
	for (std::size_t i = 0; i < sizeof(uint128_t) / sizeof(std::uint8_t); i+=2) {
		hash0 += p[i];
		hash1 += p[i+1];
		//hash += p[i];
		//hash += (p[i] << 8) + (p[i] & 0xFFFF0000);
	}

	// Add length value.
	p = (uint8_t *) &length;
	for (std::size_t i = 0; i < sizeof(std::uint32_t) / sizeof(std::uint8_t); i+=2) {
		hash0 += p[i];
		hash1 += p[i+1];
		//hash += p[i];
		//hash += (p[i] << 8) + (p[i] & 0xFFFF0000);
	}

	// Add next header field.
	hash1 += 89;

	// Add the data to the checksum.
	for (std::size_t i = 0; i < data.size() - 1; i+=2) {
		hash0 += data[i];
		hash1 += data[i+1];
		//if (i == 13 || i == 14) continue;
		//std::uint16_t add = data[i];
		//add = (add << 8) | data[i + 1];
		//hash += add;
		//hash += (uint16_t(data[i]) << 8) + data[i+1];
	}

	// Add the last data byte to the checksum if it has an odd number of bytes.
	if (data.size() % 2) {
		hash0 += data[data.size() - 1];
	}

	// Fix the carries.
	for (int i = 0; i < 4; i++) {
		std::cout << std::hex << hash0 << " / " << std::hex << hash1 << "\n";
		uint32_t h0c = hash0 >> 8;
		hash0 = (hash0 & 0xFF) + (hash1 >> 8);
		hash1 = (hash1 & 0xFF) + h0c;
	}

	return std::uint16_t((hash0 << 8) | hash1);
}

bool parser::checksum::ospf::verifyChecksum(uint128_t sourceAddress, uint128_t destAddress,
		std::uint32_t length, const parser::bytevector& data)
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