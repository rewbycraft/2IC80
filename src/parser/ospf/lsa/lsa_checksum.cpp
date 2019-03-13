//
// Created by kaj on 1-3-19.
//

#include <vector>
#include "lsa_checksum.h"


private std::pair<std::uint8_t, std::uint8_t> revertPartLSAChecksum(uint16_t& checksum, std::size_t size)
{
	uint8_t x = (checksum >> 8) & 0xFF;
	uint8_t y = checksum & 0xFF;
	uint8_t alpha = data.size() - 17;
	uint8_t beta = data.size() - 16;

	uint8_t c0 = -x - y;
	uint8_t c1 = -beta*x - alpha*y;
}

private std::pair<std::uint8_t, std::uint8_t> calcLSAChecksumPart(const parser::bytevector& data, bool verify)
{
	uint8_t c0 = 0;
	uint8_t c1 = 0;

	uint8_t pos = 0;
	for (auto &header : data) {
		if (verify || (++pos != 17 && pos != 18)) {
			c0 += header.first[i];
		}

		c1 += c0;
	}

	return { c0, c1 };
}


std::optional<std::vector<std::pair<std::uint8_t, std::uint8_t>>> parser::modifyChecksum(
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
	std::vector<uint8_t, uint8_t> targets;
	for (std::size_t& i : targetIndices) {
		targets.push_back(data[i], i);
	}

	// Determine the amount the checksum has been changed.
	std::uint16_t checksumPart = calcLSAChecksumPart(data, false);
	std::uint16_t revertedChecksum;
	std::uint16_t checksumDiff = checksum - targetChecksum;
	std::uint8_t c0_diff = (checksumDiff >> 8) & 0xFF;
	std::uint8_t c1_diff = checksumDiff & 0xFF;

	std::uint8_t div = targets[1].first - targets[0].first;
	if (interRes1 / div * div != c0_diff ||
			interRes2 / div * div != c1_diff) {
		return std::nullopt;
	}


	targets[0].first = targets[0].first+ c0_diff / div;
	targets[1].first = targets[1].first + c1_diff / div;
	return { changedVector };
}

std::uint16_t parser::calcLSAChecksum(const parser::bytevector& data)
{
	std::pair<std::uint8_t, std::uint8_t> res = calcLSAChecksumPart(data, false);
	uint8_t x = -res.second + (data.size() - 17) * res.first;
	uint8_t y =  res.second - (data.size() - 16) * res.first;

	return (((std::uint16_t) x) << 8) | y;
}

bool parser::verifyLSAChecksum(const parser::bytevector& data)
{
	std::pair<std::uint8_t, std::uint8_t> res = calcLSAChecksumPart(data, true);
	return (res.first == 0) && (res.second == 0);
}

