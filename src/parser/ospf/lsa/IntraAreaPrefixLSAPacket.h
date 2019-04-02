//
// Created by rewbycraft on 3/16/19.
//

#ifndef ATTACK_INTRAAREAPREFIXLSAPACKET_H
#define ATTACK_INTRAAREAPREFIXLSAPACKET_H

#include "LSAPacket.h"
#include "PrefixLSAPacket.h"
#include "ChecksumInterface.h"


namespace parser {
	class IntraAreaPrefixLSAPacket : public Packet, public ChecksumInterface {
	public:
		typedef struct {
			uint16_t num_prefixes;
			uint16_t referenced_lsa_type;
			uint32_t referenced_link_state_id;
			uint32_t referenced_advertising_router;
		} Header;

	private:
		Header header;
		std::vector<std::shared_ptr<PrefixLSAPacket>> prefixes;

	public:
		IntraAreaPrefixLSAPacket();
		
		explicit IntraAreaPrefixLSAPacket(const bytevector &data);
		
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
		 * @return a vector containing the pointers of the LSA prefix packets of this packet.
		 */
		const std::vector<std::shared_ptr<PrefixLSAPacket>> &getPrefixes() const;

		/**
		 * Sets the LSA prefix packets of this packet.
		 *
		 * @param prefixes the vector containing the pointers of the new LSA prefix packets.
		 */
		void setPrefixes(const std::vector<std::shared_ptr<PrefixLSAPacket>> &prefixes);

		void updateValues() override;

        std::vector<size_t> getEmptyByteIndices() override;
	};
}

BOOST_FUSION_ADAPT_STRUCT(parser::IntraAreaPrefixLSAPacket::Header, (uint16_t, num_prefixes), (uint16_t, referenced_lsa_type), (uint32_t, referenced_link_state_id), (uint32_t, referenced_advertising_router))

#endif //ATTACK_INTRAAREAPREFIXLSAPACKET_H
