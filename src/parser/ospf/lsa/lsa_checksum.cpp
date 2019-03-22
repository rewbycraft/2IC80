//
// Created by kaj on 1-3-19.
//

// tmp
//#include <iomanip>
//#include <iostream>
//#include <bitset>

#include <vector>
#include "lsa_checksum.h"


std::pair<int, int> revertPartLSAChecksum(const uint16_t& checksum, std::size_t size) {
	int x = (checksum >> 8) & 0xFF;
	int y = checksum & 0xFF;
	int alpha = int(size) - 17;
	int beta = int(size) - 16;

	int c0 = -x - y;
	int c1 = -beta*x - alpha*y;
	return { c0, c1 };
}


std::pair<int, int> calcLSAChecksumPart(const parser::bytevector& data, bool verify) {
    /*
    // Copied and modified the code from:
    // https://gitlab.labs.nic.cz/labs/bird/blob/77edab640925e8d1131f0d00b41de055129ff99f/lib/fletcher16.h
    // Note: beforehand it must hold that checksum == 0 and age == 0.

    #define MODX 4096
	int c0 = 0;
	int c1 = 0;
	int blen;
	std::size_t index = 0; // Skip the age field.
	int len = int(data.size());

	blen = len % 4;
	len -= blen;

	for (int i = 0; i < blen; i++)
		c1 += c0 += data[i++];

	do {
		blen = (len < MODX ? len : MODX);
		len -= blen;

		for (int i = 0; i < blen; i += 4)
		{
			c1 += c0 += data[index++];
			c1 += c0 += data[index++];
			c1 += c0 += data[index++];
			c1 += c0 += data[index++];
		}

		c0 %= 255;
		c1 %= 255;

	} while (len);
    /**/
	int c0 = 0;
	int c1 = 0;

	int pos = 0;
	for (auto &value : data) {
	    if (++pos == 1 || pos == 2) continue;
        if (!verify && (pos == 17 || pos == 18)) {
            c1 = (c1 + c0) % 255;

        } else {
            c0 = (c0 + value) % 255;
            c1 = (c1 + c0) % 255;
        }
	}/**/

	return { c0, c1 };
}


std::optional<std::vector<std::pair<std::uint8_t, std::size_t>>> parser::checksum::lsa::modifyChecksum(
		const parser::bytevector& data, const std::vector<std::size_t>& targetIndices,
		const std::uint16_t& targetChecksum) {
	// Determine which fields are allowed to be changed.
	std::vector<std::pair<uint8_t, size_t>> targets;
	for (std::size_t i : targetIndices) {
		targets.push_back({data[i], i});
	}

	// Determine the amount c0 and c1 were changed.
	std::pair<std::uint8_t, std::uint8_t> checksumPart = calcLSAChecksumPart(data, false);
	std::pair<std::uint8_t, std::uint8_t> revertedChecksum = revertPartLSAChecksum(targetChecksum, data.size());
	int c0_diff = checksumPart.first - revertedChecksum.first;
	int c1_diff = checksumPart.second - revertedChecksum.second;

	// Calculate part of the solution.
	int div = int(targets[0].second - targets[1].second) % 255;
	int sub_dt0 = int(targets[1].second * c0_diff - c1_diff) % 255;
	int sub_dt1 = int(targets[0].second * c0_diff + c1_diff) % 255;

	int dt0 = sub_dt0 / div;
	int dt1 = sub_dt1 / div;

	// Check if there is a solution available.
	if (dt0 * div != sub_dt0 || dt1 * div != sub_dt1) {
		return std::nullopt;
	}

	// Determine and return the result.
	targets[0].first = uint8_t(targets[0].first + dt0 / div);
	targets[1].first = uint8_t(targets[1].first + dt1 / div);
	return { targets };
}


std::uint16_t parser::checksum::lsa::calcChecksum(const parser::bytevector& data) {
	std::pair<int, int> res = calcLSAChecksumPart(data, false);
	/*
    // Copied and modified the code from:
    // https://gitlab.labs.nic.cz/labs/bird/blob/77edab640925e8d1131f0d00b41de055129ff99f/lib/fletcher16.h
	int x = int((data.size() - 17) * res.first - res.second) % 255;
	if (x <= 0) x += 255;

	int y = 510 - res.first - x;
	if (y > 255) y -= 255;
    //std::cout << "0x" << std::hex << x << std::dec << " / " << x << " / 0b " << std::bitset<8>(x) << "\n";
    //std::cout << "0x" << std::hex << y << std::dec << " / " << y << " / 0b " << std::bitset<8>(y) << "\n";

	return uint16_t((x << 8) | y);

	/**/
	int x = (-res.second + (int(data.size()) - 17) * res.first) % 255;
	int y = ( res.second - (int(data.size()) - 16) * res.first) % 255;
    if (x <= 0) x += 255;
    if (y <= 0) y += 255;

    //std::cout << "0x" << std::hex << x << std::dec << " / " << x << " / 0b " << std::bitset<8>(x) << "\n";
    //std::cout << "0x" << std::hex << y << std::dec << " / " << y << " / 0b " << std::bitset<8>(y) << "\n";

	return uint16_t((x << 8) | y);
	/**/
}

bool parser::checksum::lsa::verifyChecksum(const parser::bytevector& data) {
	std::pair<std::uint8_t, std::uint8_t> res = calcLSAChecksumPart(data, true);
	return (res.first == 0) && (res.second == 0);
}

