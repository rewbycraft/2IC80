//
// Created by kaj on 1-3-19.
//

#ifndef ATTACK_OSPF_CHECKSUM_H
#define ATTACK_OSPF_CHECKSUM_H

#include <vector>
#include "../Packet.h"
#include <optional>


namespace parser {
	namespace checksum {
		namespace ospf {
			/**
			 * Calculates the IPv6 checksum of the OPSF header.<br>
			 * <br>
			 * Notes:<ul>
			 *   <li>Note that the given data doesn't have to be in the right order.</li>
			 *   <li>The checksum field should be excluded from the {@code data} field.<br>
			 *       One can also solve this by setting the checksum field to 0.</li>
		 	 * </ul>
			 *
			 * @param sourceAddress the 128 bit IPv6 source address field.
			 * @param destAddress the 128 bit IPv6 destination address field.
			 * @param length the 32 bit IPv6 length field.
			 * @param data the data stream of the OSPF header.
			 * @return the IPv6 checksum of the OSPF header.
			 */
			std::uint16_t calcChecksum(uint128_t sourceAddress, uint128_t destAddress,
					std::uint32_t length, const bytevector& data);

			/**
			 * Verifies the IPv6 checksum of the OPSF header.<br>
			 * <br>
			 * Note that the given data doesn't have to be in the right order.
			 * The checksum field must be included in the {@code data} field.
			 *
			 * @param sourceAddress the 128 bit IPv6 source address field.
			 * @param destAddress the 128 bit IPv6 destination address field.
			 * @param length the 32 bit IPv6 length field.
			 * @param data the data stream of the OSPF header.
			 * @return {@code true} if the hash in this OSPF header was valid. {@code false} otherwise.
			 */
			bool verifyChecksum(uint128_t sourceAddress, uint128_t destAddress,
					std::uint32_t length, const bytevector& data);
		}
	}
	
}

#endif //ATTACK_OSPF_CHECKSUM_H
