//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_LINKSTATEACKNOWLEDGEMENTPACKET_H
#define ATTACK_LINKSTATEACKNOWLEDGEMENTPACKET_H

#include "lsa/LSAPacket.h"

namespace parser {
	class LinkStateAcknowledgementPacket : public Packet {
		std::vector<std::shared_ptr<LSAPacket>> lsas;
	public:
		LinkStateAcknowledgementPacket();
		
		LinkStateAcknowledgementPacket(const bytevector &data);
		
		const bytevector serialize() const override;
		
		const std::vector<std::shared_ptr<LSAPacket>> &getLsas() const;
		
		void setLsas(const std::vector<std::shared_ptr<LSAPacket>> &lsas);
		
		void toString(const std::function<void(const std::string &)> &printer) const override;
	};
}


#endif //ATTACK_LINKSTATEACKNOWLEDGEMENTPACKET_H
