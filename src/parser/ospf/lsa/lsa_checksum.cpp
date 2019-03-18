//
// Created by kaj on 1-3-19.
//

#include <vector>
#include "lsa_checksum.h"


std::pair<std::uint8_t, std::uint8_t> revertPartLSAChecksum(const uint16_t& checksum, std::size_t size)
{
	uint8_t x = (checksum >> 8) & 0xFF;
	uint8_t y = checksum & 0xFF;
	uint8_t alpha = size - 17;
	uint8_t beta = size - 16;

	uint8_t c0 = -x - y;
	uint8_t c1 = -beta*x - alpha*y;
	return { c0, c1 };
}

std::pair<std::uint8_t, std::uint8_t> calcLSAChecksumPart(const parser::bytevector& data, bool verify)
{
	uint8_t c0 = 0;
	uint8_t c1 = 0;

	uint8_t pos = 0;
	for (auto &header : data) {
		if (verify || (++pos != 17 && pos != 18)) {
			c0 += header;
		}

		c1 += c0;
	}

	return { c0, c1 };
}


std::optional<std::vector<std::pair<std::uint8_t, std::size_t>>> parser::checksum::lsa::modifyChecksum(
		const parser::bytevector& data, const std::vector<std::size_t>& targetIndices,
		const std::uint16_t& targetChecksum)
{
	/*
	std::uint8_t interRes1 = 0;
	std::uint8_t interRes2 = 0;
	for (uint8_t i = 0; i < change.size(); i++) {
		interRes1 += (targets[1].second - change[i].second) * ((*change[i].first) - newValues[i]);
		interRes2 += (change[i].second - targets[0].second) * ((*change[i].first) - newValues[i]);
	}
	*/
	// Determine which fields are allowed to be changed.
	std::vector<std::pair<uint8_t, size_t>> targets;
	for (std::size_t i : targetIndices) {
		targets.push_back({data[i], i});
	}

	// Determine the amount c0 and c1 were changed.
	std::pair<std::uint8_t, std::uint8_t> checksumPart = calcLSAChecksumPart(data, false);
	std::pair<std::uint8_t, std::uint8_t> revertedChecksum = revertPartLSAChecksum(targetChecksum, data.size());
	std::uint8_t c0_diff = checksumPart.first - revertedChecksum.first;
	std::uint8_t c1_diff = checksumPart.second - revertedChecksum.second;

	// Calculate part of the solution.
	std::uint8_t div = targets[0].second - targets[1].second;
	std::uint8_t sub_dt0 = targets[1].second * c0_diff - c1_diff;
	std::uint8_t sub_dt1 = targets[0].second * c0_diff + c1_diff;

	std::uint8_t dt0 = sub_dt0 / div;
	std::uint8_t dt1 = sub_dt1 / div;
	// Check if there is a solution available.
	if (dt0 * div != sub_dt0 || dt1 * div != sub_dt1) {
		return std::nullopt;
	}

	// Determine and return the result.
	targets[0].first = targets[0].first + dt0 / div;
	targets[1].first = targets[1].first + dt1 / div;
	return { targets };
}

std::uint16_t parser::checksum::lsa::calcChecksum(const parser::bytevector& data)
{
	std::pair<std::uint8_t, std::uint8_t> res = calcLSAChecksumPart(data, false);
	uint8_t x = -res.second + (data.size() - 17) * res.first;
	uint8_t y =  res.second - (data.size() - 16) * res.first;

	return (((std::uint16_t) x) << 8) | y;
}

bool parser::checksum::lsa::verifyChecksum(const parser::bytevector& data)
{
	std::pair<std::uint8_t, std::uint8_t> res = calcLSAChecksumPart(data, true);
	return (res.first == 0) && (res.second == 0);
}

