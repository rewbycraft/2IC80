//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_LSAPACKET_H
#define ATTACK_LSAPACKET_H

#include "../../Packet.h"
#include <memory>
#include <boost/fusion/include/adapt_struct.hpp>

namespace parser {
	class LSAPacket : public Packet {
public:
	enum Function : uint16_t { ROUTER_LSA = 0x01, NETWORK_LSA = 0x02, INTER_AREA_PREFIX_LSA = 0x03, INTER_AREA_ROUTER_LSA = 0x04, AS_EXTERNAL_LSA = 0x05, GROUP_MEMBERSHIP_LSA=0x06, NSSA_LSA = 0x07, LINK_LSA = 0x08, INTRA_AREA_PREFIX_LSA = 0x09};
	typedef struct {
		uint16_t age;
		uint8_t options : 3;
		Function function : 13;
		uint32_t id;
		uint32_t advertising_router;
		uint32_t seq;
		uint16_t checksum;
		uint16_t length;
	} __attribute__ ((__packed__)) Header;
private:
		Header header;
		std::shared_ptr<parser::Packet> subpacket;
	public:
		LSAPacket();
		
		LSAPacket(const bytevector &data);
		
		const bytevector serialize() const override;
		
		const Header &getHeader() const;
		
		void setHeader(const Header &header);
		
		const std::shared_ptr<Packet> &getSubpacket() const;
		
		void setSubpacket(const std::shared_ptr<Packet> &subpacket);
	};
}

BOOST_FUSION_ADAPT_STRUCT(parser::LSAPacket::Header, age, options, function, id, advertising_router, seq, checksum, length)

#endif //ATTACK_LSAPACKET_H
