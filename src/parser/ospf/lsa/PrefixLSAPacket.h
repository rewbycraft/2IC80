//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_PREFIXLSAPACKET_H
#define ATTACK_PREFIXLSAPACKET_H

#include "LSAPacket.h"

namespace parser {
	class PrefixLSAPacket : public Packet {
	public:
		typedef struct {
			uint8_t length;
			uint8_t options;
			uint16_t special;
			uint128_t address;
		} Header;
	private:
		Header header;
	public:
		PrefixLSAPacket();
		
		PrefixLSAPacket(const bytevector &data);
		
		const bytevector serialize() const override;
		
		const Header &getHeader() const;
		
		void setHeader(const Header &header);
	};
	
	std::pair<std::shared_ptr<PrefixLSAPacket>, const bytevector> extractPrefix(const bytevector& input);
}


#endif //ATTACK_PREFIXLSAPACKET_H
