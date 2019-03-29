//
// Created by kaj on 1-3-19.
//

#ifndef ATTACK_LSA_CHECKSUM_H
#define ATTACK_LSA_CHECKSUM_H

#include <vector>
#include "../../Packet.h"

namespace parser {
	namespace checksum {
		namespace lsa {
			/**
			 * Modifies the given {@code targets} fields to compensate for the changed {@code change} fields
			 * to keep the same checksum. Note that the {@code change} fields are only replaced
			 * by {@code newValues} if, and only if, there exists values for the {@code targets} fields
			 * such that the checksum remains equal after the change. Otherwise no input values are changed.
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
			 * @return {@code true} if the checksum is valid. {@code false} otherwise.
			 */
			std::optional<std::vector<std::pair<std::uint8_t, std::size_t>>> modifyChecksum(
					const parser::bytevector &data, const std::vector<std::size_t> &targetIndices,
					const std::uint16_t &targetChecksum);

			/**
			 * Calculates the LSA checksum over the given data.
			 * Note that the given data MUST be in the right order.
			 *
			 * @param data a vector of pairs where the first element denotes
			 *     a pointer each (sub) header, and the second element denotes
			 *     the length of that header.
			 * @return the checksum of the LSA header.
			 */
			std::uint16_t calcChecksum(const parser::bytevector &data);

			/**
			 * Verifies the LSA checksum.
			 * Note that the given {@code data} MUST be in the right order.
			 *
			 * @param data
			 * @return
			 */
			bool verifyChecksum(const bytevector &data);
		}
	}
}
#endif //ATTACK_LSA_CHECKSUM_H
