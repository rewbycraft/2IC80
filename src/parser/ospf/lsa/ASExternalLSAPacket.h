//
// Created by rewbycraft on 3/18/19.
//

#ifndef ATTACK_ASEXTERNALLSAPACKET_H
#define ATTACK_ASEXTERNALLSAPACKET_H

#include "LSAPacket.h"
#include "PrefixLSAPacket.h"
#include "ChecksumInterface.h"


namespace parser {
	class ASExternalLSAPacket : public Packet, ChecksumInterface {
	public:
		typedef struct {
			uint32_t _options;
			uint128_t forwarding_address;
			uint32_t external_route_tag;
			uint32_t referenced_link_state_id;
		} Header;

	private:
		Header header;
		std::shared_ptr<parser::PrefixLSAPacket> prefix;

	public:
		ASExternalLSAPacket();

		explicit ASExternalLSAPacket(const bytevector &data);
		
		const bytevector serialize() const override;

		void toString(const std::function<void(const std::string &)> &printer) const override;

		/**
		 * @return the header of this packet.
		 */
		const Header &getHeader() const;

		/**
		 * Sets the header of this packet.
		 *
		 * @param header the new header.
		 */
		void setHeader(const Header &header);

		/**
		 * @return the pointers of the LSA prefix packet of this packet.
		 */
		const std::shared_ptr<PrefixLSAPacket> &getPrefix() const;

		/**
		 * Sets the LSA prefix packet of this packet.
		 *
		 * @param prefixes the pointer of the new LSA prefix packets.
		 */
		void setPrefix(const std::shared_ptr<PrefixLSAPacket> &prefix);

		void updateValues() override;

		std::vector<size_t> getEmptyByteIndices() override;
	};
}


#endif //ATTACK_ASEXTERNALLSAPACKET_H
