//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_PREFIXLSAPACKET_H
#define ATTACK_PREFIXLSAPACKET_H

#include "LSAPacket.h"
#include "ChecksumInterface.h"

namespace parser {
class PrefixLSAPacket : public Packet, public ChecksumInterface {
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
		
		void toString(const std::function<void(const std::string &)> &printer) const override;
		
		void updateValues() override;

		std::vector<size_t> getEmptyByteIndices() override;
	};
	
	std::pair<std::shared_ptr<PrefixLSAPacket>, const bytevector> extractPrefix(const bytevector& input);
}

BOOST_FUSION_ADAPT_STRUCT(parser::PrefixLSAPacket::Header, (uint8_t, length), (uint8_t, options), (uint16_t, special), (uint128_t, address))

#endif //ATTACK_PREFIXLSAPACKET_H
