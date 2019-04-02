//
// Created by kaj on 1-3-19.
//

#include <vector>
#include "lsa_checksum.h"


/**
 * @param i the input value.
 * @param mod the modulus.
 * @return {@code i} mod {@code mod}.
 */
int mod(int i, int mod) {
    int rtn = i % mod;
    if (rtn < 0) rtn += mod;
    return rtn;
}

/**
 * Reverts the given checksum into {@code c0} and {@code c1}.
 *
 * @param checksum the original fletcher-16 LSA checksum value.
 * @param size the size of the serialized packet.
 * @return the pair containing the {@code c0} and {@code c1} values.
 */
std::pair<int, int> revertPartLSAChecksum(const uint16_t &checksum, std::size_t size) {
	int x = (checksum >> 8) & 0xFF;
	int y = checksum & 0xFF;
	int l = int(size % 255);

    int c0 = mod(-x - y, 255);
	int c1 = mod((16 - l) * x + (17 - l) * y, 255);
	return { c0, c1 };
}

/**
 * Calculates the main part of the fletcher-16 LSA checksum for the
 * LSA checksum field.<br>
 * <br>
 * Only includes the checksum field if {@code verify == true}.<br>
 * <br>
 * The {@code c0} and {@code c1} values are actually 8-bit integers,
 * but an {@code int} is used instead of {@code std::uint8_t} since
 * this is much easier with overflow handeling.
 *
 * @param data the serialized LSA packet.
 * @param verify whether the checksum part must be determined or verified.
 * @return the pair containing the {@code c0} and {@code c1} values.
 */
std::pair<int, int> calcLSAChecksumPart(const parser::bytevector& data, bool verify) {
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
	}

	return { c0, c1 };
}

/**
 * Extended euclidean algorithm.<br>
 * <a href="https://www.geeksforgeeks.org/modular-division/">source</a><br>
 * <br>
 * Calculates the greatest common divisor (gcd) of {@code a} and {@code b}.
 * <br>
 * Also modifies the values {@code x} and {@code y} such that
 * {@code a*x + b*y = gcd(a, b)}.
 *
 * @param a the first input value.
 * @param b the second input value.
 * @param x cannot be {@code nullptr}.
 * @param y cannot be {@code nullptr}.
 *
 * @return the {@code gcd(a, b)}.
 *
 * @modifies x,y such that {@code a*x + b*y = gcd(a, b)} holds.
 */
int gcdExtended(int a, int b, int *x, int *y) {
    // Base Case
    if (a == 0) {
        *x = 0, *y = 1;
        return b;
    }

    // Recursively determine the result.
    int x1, y1;
    int gcd = gcdExtended(b%a, a, &x1, &y1);

    // Update x and y using results of recursive call
    *x = y1 - (b/a) * x1;
    *y = x1;

    return gcd;
}

std::uint16_t parser::checksum::lsa::calcChecksum(const parser::bytevector& data) {
    std::pair<int, int> res = calcLSAChecksumPart(data, false);
    int x = mod(-res.second + (int(data.size()) - 17) * res.first, 255);
    int y = mod( res.second - (int(data.size()) - 16) * res.first, 255);
    return uint16_t((x << 8) | y);
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

    int delta_c0 = checksumPart.first - revertedChecksum.first;
    int delta_c1 = checksumPart.second - revertedChecksum.second;
    int l = int(data.size() % 255);
    std::size_t i, j;
    int x, y, inv;
    bool found = false;

    for (i = 0; i < targets.size(); i++) {
        for (j = i + 1; j < targets.size(); j++) {
            x = int(targets[i].first % 255);
            y = int(targets[j].first % 255);

            // Determine the divisor.
            int div = y - x;

            // Calculate the inverse for the divisor.
            int dummy;
            int gcd = gcdExtended(div, 255, &inv, &dummy);
            // If it exists, set found flag, calculate delta's and break.
            if (gcd == 1) {
                found = true;
                break;
            }
        }
        if (found) break;
    }
    int delta_x = mod( (l - y) * delta_c0 - delta_c1, 255) * inv;
    int delta_y = mod(-(l - x) * delta_c0 + delta_c1, 255) * inv;

    if (!found) return { };
    targets[i].second = uint8_t(mod(delta_x + targets[i].second, 255));
    targets[j].second = uint8_t(mod(delta_y + targets[j].second, 255));

    return { targets };
}

bool parser::checksum::lsa::verifyChecksum(const parser::bytevector& data) {
	std::pair<std::uint8_t, std::uint8_t> res = calcLSAChecksumPart(data, true);
	return (res.first == 0) && (res.second == 0);
}

