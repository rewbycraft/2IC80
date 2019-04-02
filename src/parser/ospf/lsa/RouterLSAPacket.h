//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_ROUTERLSAPACKET_H
#define ATTACK_ROUTERLSAPACKET_H

#include "LSAPacket.h"
#include "ChecksumInterface.h"


namespace parser {
	class RouterLSAPacket : public Packet, public ChecksumInterface {
	public:
		typedef struct {
			uint32_t options;
		} Header;
		typedef struct {
			uint8_t type;
			uint8_t unused;
			uint16_t metric;
			uint32_t interface_id;
			uint32_t neighbor_interface_id;
			uint32_t neighbor_router_id;
		} InterfaceHeader;

	private:
		Header header;
		std::vector<InterfaceHeader> interfaces;

	public:
		RouterLSAPacket();
		
		explicit RouterLSAPacket(const bytevector &data);
		
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
		 * @return a vector containing the headers of the interfaces.
		 */
		const std::vector<InterfaceHeader> &getInterfaces() const;

		/**
		 * Sets the headers of the interfaces.
		 *
		 * @param interfaces the vector containing the new headers.
		 */
		void setInterfaces(const std::vector<InterfaceHeader> &interfaces);

		void toString(const std::function<void(const std::string &)> &printer) const override;
		
		void updateValues() override;

		std::vector<size_t> getEmptyByteIndices() override;
	};
}

BOOST_FUSION_ADAPT_STRUCT(parser::RouterLSAPacket::Header, (uint32_t, options))

BOOST_FUSION_ADAPT_STRUCT(parser::RouterLSAPacket::InterfaceHeader, (uint8_t, type), (uint8_t, unused), (uint16_t, metric), (uint32_t, interface_id), (uint32_t, neighbor_interface_id), (uint32_t, neighbor_router_id))

#endif //ATTACK_ROUTERLSAPACKET_H
