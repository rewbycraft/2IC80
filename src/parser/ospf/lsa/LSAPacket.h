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
		uint16_t _options;
		uint32_t id;
		uint32_t advertising_router;
		int32_t seq;
		uint16_t checksum;
		uint16_t length;

		/**
		 * @return the option field of this header.
		 */
		const uint8_t getOptions() const {
			return (_options >> 13) & generateMask<uint16_t>(3);
		}

		/**
		 * Sets the options of this header.
		 *
		 * @param options the new options.
		 */
		void setOptions(const uint8_t& options) {
			_options = (_options & generateMask<uint16_t>(13)) + ((static_cast<uint16_t>(options) & generateMask<uint16_t>(3)) << 13);
		}

		/**
		 * @return the function ID of this header.

		 * @see LSAPacket::Function
		 */
		const Function getFunction() const {
			return static_cast<Function>(_options & generateMask<uint16_t>(13));
		}

		/**
		 * Sets the function ID of this header.
		 *
		 * @param function the new function ID.
		 */
		void setFunction(const Function& function) {
			_options = (_options & (generateMask<uint16_t>(3) << 13)) + function;
		}
	} Header;

	private:
		Header header;
		std::shared_ptr<parser::Packet> subpacket;

	public:
		LSAPacket();

		explicit LSAPacket(const bytevector &data);
		
		const bytevector serialize() const override;

		/**
		 * @return the header of this packet.
		 */
		const Header &getHeader() const;

		/**
		 * Sets the header of this packet.
		 *
		 * @param header the new header of this packet.
		 */
		void setHeader(const Header &header);

		/**
		 * @return the subpacket of this packet.
		 */
		const std::shared_ptr<Packet> &getSubpacket() const;

		/**
		 * Sets the subpacket of this packet.
		 *
		 * @param subpacket the new subpacket.
		 */
		void setSubpacket(const std::shared_ptr<Packet> &subpacket);

		void toString(const std::function<void(const std::string &)> &printer) const override;
		
		void updateValues() override;

		/**
		 * Calculates and updates the checksum of this entire packet.
		 */
		void updateChecksum();

		/**
		 * Modifies the packet such that the checksum will be equal to the given value.
		 *
		 * @param targetChecksum the checksum the new packet will have when computed.
		 * @return a new {@code LSAPacket} of which all fields except the
		 *     target fields are equal to the source packet, and the
		 *     checksum is equal to {@code targetChecksum}.
		 */
        const std::optional<std::shared_ptr<LSAPacket>> modToChecksum(uint16_t targetChecksum);

	};
}

BOOST_FUSION_ADAPT_STRUCT(parser::LSAPacket::Header, (uint16_t,age), (uint16_t,_options), (uint32_t,id), (uint32_t, advertising_router), (int32_t,seq), (uint16_t,checksum), (uint16_t,length))

#endif //ATTACK_LSAPACKET_H
