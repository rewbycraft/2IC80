//
// Created by kaj on 1-3-19.
//

#ifndef ATTACK_LSA_CHECKSUM_H
#define ATTACK_LSA_CHECKSUM_H

#include <vector>
#include "../../Packet.h"

namespace parser {
	/**
	 *
	 * @param targets a vector of pairs where the first element denotes the byte
	 *     which can be modified, and the second the index of this byte relative
	 *     to the beginning of the LSA header (first byte is at position 1).
	 * @param change a vector of pairs where the first element denotes the byte
	 *     which should be modified, and the second the index of this byte relative
	 *     to the beginning of the LSA header.
	 * @param newValues a vector denoting the new values to set for the bytes
	 *     specified in {@code changed}.
	 *
	 * @return
	 */
	bool modifyChecksum(
			std::vector<std::pair<std::uint8_t *, std::uint8_t>> targets,
			std::vector<std::pair<std::uint8_t *, std::uint8_t>> change,
			std::vector<std::uint8_t> newValues);

	/**
	 * @param data a vector of pairs where the first element denotes
	 *     a pointer each (sub) header, and the second element denotes
	 *     the length of that header.
	 * @return the checksum of the LSA header.
	 */
	uint16_t calcLSAChecksum(std::vector<std::pair<std::uint8_t *, std::uint8_t>> data);

	bool verifyLSAChecksum(std::vector<std::pair<std::uint8_t *, std::uint8_t>> data);
}
#endif //ATTACK_LSA_CHECKSUM_H
