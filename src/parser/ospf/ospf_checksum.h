//
// Created by kaj on 1-3-19.
//

#ifndef ATTACK_OSPF_CHECKSUM_H
#define ATTACK_OSPF_CHECKSUM_H

#include <vector>
#include "../Packet.h"


namespace parser {
	/**
	 * @param data a vector of pairs where the first element denotes
	 *     a pointer each (sub) header, and the second element denotes
	 *     the length of that header.
	 * @return the hash of the OSPF header.
	 */
	std::uint16_t calcOSPFChecksum(uint128_t sourceAddress,
			uint128_t destAddress, std::uint32_t length,
			std::vector<std::pair<std::uint8_t *, std::uint8_t>> data);

	/**
	 * @param data a vector of pairs where the first element denotes
	 *     a pointer each (sub) header, and the second element denotes
	 *     the length of that header.
	 * @return {@code true} if the hash in this OSPF header was valid. {@code false} otherwise.
	 */
	bool checkOSPFChecksum(std::vector<std::pair<std::uint8_t *, std::uint8_t>> data);


}

#endif //ATTACK_OSPF_CHECKSUM_H
