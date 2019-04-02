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
			 * Calculates the LSA checksum over the given data.
			 * Note that the given data MUST be in the right order.
			 *
			 * @param data a serialized bytevector of a LSA packet.
			 * @return the LSA fletcher-16 checksum of the LSA packet.
			 */
			std::uint16_t calcChecksum(const parser::bytevector &data);

			/**
			 * Returns a suggestion to modify the given {@code data} bytevector such that
			 * calculating the LSA fletcher-16 checksum will result in {@code targetChecksum}
			 * when only using the indices {@code targetIndices} in {@code data}.
			 *
			 * @param data the bytevector representing the serialisation of the LSA packet.
			 * @param targetIndices the indices of the bytes in {@code data} which are allowed
			 *     to be modified.
			 * @param targetChecksum the checksum which should be foreged.
			 * @return A vector containing pairs where the first value contains the pointer and
			 *     the second the new data which should be changed in the data stream, or
			 *     {@link std::nullopt} if it is not possible to do so.
			 */
			std::optional<std::vector<std::pair<std::size_t, std::uint8_t>>> modifyChecksum(
					const parser::bytevector &data, const std::vector<std::size_t> &targetIndices,
					const std::uint16_t &targetChecksum);

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
