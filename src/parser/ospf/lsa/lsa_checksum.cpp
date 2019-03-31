//
// Created by kaj on 1-3-19.
//

// tmp
//#include <iomanip>
//#include <iostream>
//#include <bitset>

#include <vector>
#include "lsa_checksum.h"


int mod(int i, int mod) {
    int rtn = i % mod;
    if (rtn < 0) rtn += mod;
    return rtn;
}

std::pair<int, int> revertPartLSAChecksum(const uint16_t &checksum, std::size_t size) {
	int x = (checksum >> 8) & 0xFF;
	int y = checksum & 0xFF;
	int l = int(size % 255);

    int c0 = mod(-x - y, 255);
	int c1 = mod((16 - l) * x + (17 - l) * y, 255);
	/*
	int alpha = int(size) - 17;
	int beta = int(size) - 16;

	int c0 = (-x - y) % 255;
	if (c0 < 0) c0 += 255;
	int c1 = (-beta*x - alpha*y) % 255;
	if (c1 < 0) c0 += 255;
    */
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

/**
 * Extended euclidean algorithm.
 * Source: https://www.geeksforgeeks.org/modular-division/
 *
 * @param a
 * @param b
 * @param x
 * @param y
 * @return
 */
int gcdExtended(int a, int b, int *x, int *y) {
    // Base Case
    if (a == 0)
    {
        *x = 0, *y = 1;
        return b;
    }

    int x1, y1; // To store results of recursive call
    int gcd = gcdExtended(b%a, a, &x1, &y1);

    // Update x and y using results of recursive
    // call
    *x = y1 - (b/a) * x1;
    *y = x1;

    return gcd;
}


std::optional<std::vector<std::pair<std::size_t, std::uint8_t>>> parser::checksum::lsa::modifyChecksum(
		const parser::bytevector& data, const std::vector<std::size_t>& targetIndices,
		const std::uint16_t& targetChecksum) {
	// Determine which fields are allowed to be changed.
	std::vector<std::pair<std::size_t, std::uint8_t>> targets;
	for (std::size_t i : targetIndices) {
		targets.emplace_back(i, data[i]);
	}

    // Determine the amount c0 and c1 were changed.
    std::pair<std::uint8_t, std::uint8_t> checksumPart = calcLSAChecksumPart(data, false);
    std::pair<std::uint8_t, std::uint8_t> revertedChecksum = revertPartLSAChecksum(targetChecksum, data.size());

    int delta_c0 = (checksumPart.first - revertedChecksum.first) % 255;
    int delta_c1 = (checksumPart.second - revertedChecksum.second) % 255;
    int l = int(data.size() % 255);
    std::size_t i, j;
    int x, y, div, inv;

    bool found = false;
    for (i = 0; i < targets.size(); i++) {
        for (j = i + 1; j < targets.size(); j++) {
            x = int(targets[i].first % 255);
            y = int(targets[j].first % 255);

            // Determine the division.
            //int div = (y - x) * l;
            div = mod((y - x) * (l - 1), 255);
            // Calculate the inverse for the division.
            int dummy;
            int gcd = gcdExtended(div, 255, &inv, &dummy);
            // Check if inverse exists.
            if (gcd == 1) {
                found = true;
                break;
            }
        }
        if (found) break;
    }

    if (!found) return { };
    //int delta_x = mod(-(l - y) * delta_c0 + y * delta_c1, 255) * inv;
    //int delta_y = mod( (l - x) * delta_c0 - x * delta_c1, 255) * inv;
    int delta_x = mod(-(l - y - 1) * delta_c0 + y * delta_c1, 255) * inv;
    int delta_y = mod( (l - x - 1) * delta_c0 - x * delta_c1, 255) * inv;

    targets[i].second = uint8_t(mod(delta_x + targets[i].second, 255));
    targets[j].second = uint8_t(mod(delta_y + targets[j].second, 255));

    return { targets };

    /*
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
    return { targets };*/
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
	int x = mod(-res.second + (int(data.size()) - 17) * res.first, 255);
	int y = mod( res.second - (int(data.size()) - 16) * res.first, 255);

    //std::cout << "0x" << std::hex << x << std::dec << " / " << x << " / 0b " << std::bitset<8>(x) << "\n";
    //std::cout << "0x" << std::hex << y << std::dec << " / " << y << " / 0b " << std::bitset<8>(y) << "\n";

	return uint16_t((x << 8) | y);
	/**/
}

bool parser::checksum::lsa::verifyChecksum(const parser::bytevector& data) {
	std::pair<std::uint8_t, std::uint8_t> res = calcLSAChecksumPart(data, true);
	return (res.first == 0) && (res.second == 0);
}

