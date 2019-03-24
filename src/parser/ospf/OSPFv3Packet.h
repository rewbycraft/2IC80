//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_OSPFV3PACKET_H
#define ATTACK_OSPFV3PACKET_H

#include "../Packet.h"
#include <memory>
#include <tins/tins.h>
#include <type_traits>
#include <boost/fusion/include/adapt_struct.hpp>

namespace parser {
	class OSPFv3Packet : public Packet {
	public:
		enum Type : uint8_t { HELLO = 1, DATABASE_DESCRIPTION = 2, LINK_STATE_REQUEST = 3, LINK_STATE_UPDATE = 4, LINK_STATE_ACK = 5 };
		static_assert(sizeof(Type) == sizeof(uint8_t), "Type is not 8 bits in size.");

		typedef struct {
			uint8_t version;
			Type type;
			uint16_t packet_length;
			uint32_t router_id;
			uint32_t area_id;
			uint16_t checksum;
			uint8_t instance_id;
			uint8_t reserved;
		} Header;
		
	private:
		Header header;
		std::shared_ptr<Packet> subpacket;
		uint128_t source = 0, dest = 0;
	public:
		OSPFv3Packet();
		
		OSPFv3Packet(const bytevector& data);
		
		const bytevector serialize() const override;
		
		const Header &getHeader() const;
		
		void setHeader(const Header &header);
		
		const std::shared_ptr<Packet> &getSubpacket() const;
		
		void setSubpacket(const std::shared_ptr<Packet> &subpacket);
		
		void recomputeChecksum(const Tins::IPv6& v6Header);
		
		void toString(const std::function<void(const std::string&)>&) const override;
		
		void updateValues() override;
		
		uint128_t getSource() const;
		
		void setSource(uint128_t source);
		
		uint128_t getDest() const;
		
		void setDest(uint128_t dest);
		
		void transmit() const;
	};
}

BOOST_FUSION_ADAPT_STRUCT(parser::OSPFv3Packet::Header, (uint8_t, version), (parser::OSPFv3Packet::Type, type), (uint16_t, packet_length), (uint32_t, router_id), (uint32_t, area_id), (uint16_t, checksum), (uint8_t, instance_id), (uint8_t, reserved))


#endif //ATTACK_OSPFV3PACKET_H
