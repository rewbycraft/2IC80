//
// Created by kaj on 1-3-19.
//

#ifndef ATTACK_OSPF_CHECKSUM_H
#define ATTACK_OSPF_CHECKSUM_H

#include <vector>
#include "../Packet.h"
#include <optional>


namespace parser {
	/**
	 * Calculates the OSPF checksum.
	 * Note that the given data doesn't have to be in the right order.
	 * The checksum field should be excluded from the {@code data} field.
	 *
	 * @param sourceAddress the 128 bit IPv6 source address field.
	 * @param destAddress the 128 bit IPv6 destination address field.
	 * @param length the 32 bit IPv6 length field.
	 * @param data the data stream of the OSPF header.
	 * @return the hash of the OSPF header.
	 */
	std::uint16_t calcOSPFChecksum(uint128_t sourceAddress,
			uint128_t destAddress, std::uint32_t length, parser::bytevector data);

	/**
	 * Verifies the OSPF checksum.
	 * Note that the given data doesn't have to be in the right order.
	 * The checksum field must be included in the {@code data} field.
	 *
	 * @param sourceAddress the 128 bit IPv6 source address field.
	 * @param destAddress the 128 bit IPv6 destination address field.
	 * @param length the 32 bit IPv6 length field.
	 * @param data the data stream of the OSPF header.
	 * @return {@code true} if the hash in this OSPF header was valid. {@code false} otherwise.
	 */
	bool verifyOSPFChecksum(uint128_t sourceAddress,
	                                uint128_t destAddress, std::uint32_t length, parser::bytevector data);


}

#endif //ATTACK_OSPF_CHECKSUM_H
