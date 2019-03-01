//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_ROUTERLSAPACKET_H
#define ATTACK_ROUTERLSAPACKET_H

#include "LSAPacket.h"

namespace parser {
	class RouterLSAPacket : public Packet {
	public:
		typedef struct {
			uint8_t preoptions;
			uint8_t options : 3;
			LSAPacket::Function function : 13;
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
		
		RouterLSAPacket(const bytevector &data);
		
		const bytevector serialize() const override;
	
		const Header &getHeader() const;
		
		void setHeader(const Header &header);
		
		const std::vector<InterfaceHeader> &getInterfaces() const;
		
		void setInterfaces(const std::vector<InterfaceHeader> &interfaces);
	};
}


#endif //ATTACK_ROUTERLSAPACKET_H
