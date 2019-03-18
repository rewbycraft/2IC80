//
// Created by rewbycraft on 3/16/19.
//

#ifndef ATTACK_INTRAAREAPREFIXLSAPACKET_H
#define ATTACK_INTRAAREAPREFIXLSAPACKET_H

#include "LSAPacket.h"
#include "PrefixLSAPacket.h"

namespace parser {
	class IntraAreaPrefixLSAPacket : public parser::Packet {
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
		
		IntraAreaPrefixLSAPacket(const bytevector &data);
		
		const bytevector serialize() const override;
		
		void toString(const std::function<void(const std::string &)> &printer) const override;
		
		const Header &getHeader() const;
		
		void setHeader(const Header &header);
		
		const std::vector<std::shared_ptr<PrefixLSAPacket>> &getPrefixes() const;
		
		void setPrefixes(const std::vector<std::shared_ptr<PrefixLSAPacket>> &prefixes);
		
		void updateValues() override;
	};
}

BOOST_FUSION_ADAPT_STRUCT(parser::IntraAreaPrefixLSAPacket::Header, (uint16_t, num_prefixes), (uint16_t, referenced_lsa_type), (uint32_t, referenced_link_state_id), (uint32_t, referenced_advertising_router))

#endif //ATTACK_INTRAAREAPREFIXLSAPACKET_H
