//
// Created by kaj on 1-3-19.
//

#include <vector>
#include "lsa_checksum.h"

bool parser::modifyChecksum(
		std::vector<std::pair<std::uint8_t *, std::uint8_t>> targets,
		std::vector<std::pair<std::uint8_t *, std::uint8_t>> change,
		std::vector<std::uint8_t> newValues)
{
	std::uint8_t interRes1 = 0;
	std::uint8_t interRes2 = 0;
	for (uint8_t i = 0; i < change.size(); i++) {
		//uint8_t newMet = (i < newValues.size() ? newValues[i] : 0);
		interRes1 += (targets[1].second - change[i].second) * ((*change[i].first) - newValues[i]);
		interRes2 += (change[i].second - targets[0].second) * ((*change[i].first) - newValues[i]);
	}

	std::uint8_t div = (*targets[1].first) - (*targets[0].first);
	if (interRes1 / div * div != interRes1 ||
			interRes2 / div * div != interRes2) {
		return false;
	}

	(*targets[0].first) = (*targets[0].first) + interRes1 / div;
	(*targets[1].first) = (*targets[1].first) + interRes2 / div;
	return true;
}


std::uint16_t parser::calcLSAChecksum(std::vector<std::pair<std::uint8_t *, std::uint8_t>> data)
{
	uint8_t c0 = 0;
	uint8_t c1 = 0;

	uint8_t pos = 0;
	for (auto &header : data) {
		for (std::uint8_t i = 0; i < header.second; i++) {
			if (++pos == 17 || pos == 18) {
				c1 += c0;

			} else {
				c0 += header.first[i];
				c1 += c0;
			}
		}
	}

	uint8_t x = -c0 + (pos - 17) * c0;
	uint8_t y =  c0 - (pos - 16) * c0;

	return (((std::uint16_t) x) << 8) | y;
}

bool parser::verifyLSAChecksum(std::vector<std::pair<std::uint8_t *, std::uint8_t>> data)
{
	uint8_t c0 = 0;
	uint8_t c1 = 0;

	uint8_t pos = 0;
	for (auto &header : data) {
		for (std::uint8_t i = 0; i < header.second; i++) {
			c0 += header.first[i];
			c1 += c0;
		}
	}

	return (c0 == 0) && (c1 == 0);
}

