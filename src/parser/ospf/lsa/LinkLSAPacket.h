//
// Created by rewbycraft on 3/18/19.
//

#ifndef ATTACK_LINKLSAPACKET_H
#define ATTACK_LINKLSAPACKET_H

#include "LSAPacket.h"
#include "PrefixLSAPacket.h"

namespace parser {
	class LinkLSAPacket : public parser::Packet {
	public:
		typedef struct {
			uint32_t _options;
			uint128_t lladdr;
			uint32_t num_prefixes;
		} Header;
	private:
		Header header;
		std::vector<std::shared_ptr<parser::PrefixLSAPacket>> prefixes;
	public:
		LinkLSAPacket();
		
		LinkLSAPacket(const bytevector &data);
		
		const bytevector serialize() const override;
		
		void toString(const std::function<void(const std::string &)> &printer) const override;
		
		const Header &getHeader() const;
		
		void setHeader(const Header &header);
		
		const std::vector<std::shared_ptr<PrefixLSAPacket>> &getPrefixes() const;
		
		void setPrefixes(const std::vector<std::shared_ptr<PrefixLSAPacket>> &prefixes);
	};
}

BOOST_FUSION_ADAPT_STRUCT(parser::LinkLSAPacket::Header, (uint32_t, _options), (uint128_t, lladdr), (uint32_t, num_prefixes))

#endif //ATTACK_LINKLSAPACKET_H
