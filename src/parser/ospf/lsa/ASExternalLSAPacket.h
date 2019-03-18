//
// Created by rewbycraft on 3/18/19.
//

#ifndef ATTACK_ASEXTERNALLSAPACKET_H
#define ATTACK_ASEXTERNALLSAPACKET_H

#include "LSAPacket.h"
#include "PrefixLSAPacket.h"

namespace parser {
	class ASExternalLSAPacket : public Packet {
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
		
		ASExternalLSAPacket(const bytevector &data);
		
		const bytevector serialize() const override;
		
		void toString(const std::function<void(const std::string &)> &printer) const override;
		
		const Header &getHeader() const;
		
		void setHeader(const Header &header);
		
		const std::shared_ptr<PrefixLSAPacket> &getPrefix() const;
		
		void setPrefix(const std::shared_ptr<PrefixLSAPacket> &prefix);
		
		void updateValues() override;
	};
}


#endif //ATTACK_ASEXTERNALLSAPACKET_H
