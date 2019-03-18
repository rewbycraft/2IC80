//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_HELLOPACKET_H
#define ATTACK_HELLOPACKET_H

#include "../Packet.h"
#include <boost/fusion/include/adapt_struct.hpp>

namespace parser {
	class HelloPacket : public Packet {
	public:
		typedef struct {
			uint32_t interface_id;
			uint32_t options;
			uint16_t hello_interval;
			uint16_t router_dead_interval;
			uint32_t designated_router_id;
			uint32_t backup_designated_router_id;
		} Header;
	private:
		std::vector<uint32_t> neighbors;
		Header header;
	public:
		
		HelloPacket();
		
		HelloPacket(const bytevector &data);
		
		const bytevector serialize() const override;
		
		const std::vector<uint32_t> &getNeighbors() const;
		
		void setNeighbors(const std::vector<uint32_t> &neighbors);
		
		const Header &getHeader() const;
		
		void setHeader(const Header &header);
		
		void toString(const std::function<void(const std::string &)> &printer) const override;
	};
}

BOOST_FUSION_ADAPT_STRUCT(parser::HelloPacket::Header, (uint32_t, interface_id), (uint32_t, options), (uint16_t, hello_interval), (uint16_t, router_dead_interval), (uint32_t, designated_router_id), (uint32_t, backup_designated_router_id))

#endif //ATTACK_HELLOPACKET_H
