//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_LINKSTATEREQUESTPACKET_H
#define ATTACK_LINKSTATEREQUESTPACKET_H

#include "Packet.h"
#include "LSAPacket.h"

namespace parser {
	class LinkStateRequestPacket : public Packet {
	public:
		typedef struct {
			uint16_t empty;
			uint8_t options : 3;
			LSAPacket::Function function : 13;
			uint32_t id;
			uint32_t advertising_router;
		} RequestedLSA;
	private:
		std::vector<RequestedLSA> requestedLsas;
	public:
		LinkStateRequestPacket();
		
		LinkStateRequestPacket(const bytevector &data);
		
		const bytevector serialize() const override;
		
		const std::vector<RequestedLSA> &getRequestedLsas() const;
		
		void setRequestedLsas(const std::vector<RequestedLSA> &requestedLsas);
	};
}


#endif //ATTACK_LINKSTATEREQUESTPACKET_H
