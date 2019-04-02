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
	class OSPFv3Packet : public Packet, public std::enable_shared_from_this<OSPFv3Packet> {
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

		explicit OSPFv3Packet(const bytevector& data);
		
		const bytevector serialize() const override;

		/**
		 * @return the header of this OSPF packet.
		 */
		const Header &getHeader() const;

		/**
		 * Sets the header of this OSPF packet.
		 *
		 * @param header the new header.
		 */
		void setHeader(const Header &header);

		/**
		 * @return a pointer to the subpacket of this OSPF packet.
		 */
		const std::shared_ptr<Packet> &getSubpacket() const;

		/**
		 * Sets the subpacket of this OSPF packet.
		 *
		 * @param subpacket the new subpacket.
		 */
		void setSubpacket(const std::shared_ptr<Packet> &subpacket);

		/**
		 * Recomputes the checksum in the header of this OSPF packet
		 * using the given IPv6 header.
		 *
		 * @param v6Header the IPv6 header useded in the checksum calculations.
		 */
		void recomputeChecksum(const Tins::IPv6& v6Header);

		/**
		 * @return the source IPv6 address of this OSPF packet.
		 */
		uint128_t getSource() const;

		/**
		 * Sets the IPv6 source of this OSPF packet.
		 *
		 * @param source the new IPv6 source.
		 */
		void setSource(uint128_t source);

		/**
		 * @return the destination IPv6 address of this OSPF packet.
		 */
		uint128_t getDest() const;

		/**
		 * Sets the IPv6 destination of this OSPF packet.
		 *
		 * @param dest the new IPv6 destination.
		 */
		void setDest(uint128_t dest);

		/**
		 * Sets a matching destination IPv6 address for the
		 * current IPv6 source address.
		 */
		void setSourceFromDest();

		/**
		 * Transmits the packet over the link with the current configuration.
		 */
		void transmit() const;

		void toString(const std::function<void(const std::string &)> &printer) const override;

		void updateValues() override;
	};
}

BOOST_FUSION_ADAPT_STRUCT(parser::OSPFv3Packet::Header, (uint8_t, version), (parser::OSPFv3Packet::Type, type), (uint16_t, packet_length), (uint32_t, router_id), (uint32_t, area_id), (uint16_t, checksum), (uint8_t, instance_id), (uint8_t, reserved))


#endif //ATTACK_OSPFV3PACKET_H
