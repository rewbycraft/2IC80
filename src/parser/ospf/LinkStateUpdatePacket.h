//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_LINKSTATEUPDATEPACKET_H
#define ATTACK_LINKSTATEUPDATEPACKET_H

#include "lsa/LSAPacket.h"

namespace parser {
	class LinkStateUpdatePacket : public Packet {
	private:
		std::vector<std::shared_ptr<LSAPacket>> lsas;
	public:
		LinkStateUpdatePacket();
		
		LinkStateUpdatePacket(const bytevector &data);
		
		const bytevector serialize() const override;
		
		const std::vector<std::shared_ptr<LSAPacket>> &getLsas() const;
		
		void setLsas(const std::vector<std::shared_ptr<LSAPacket>> &lsas);
	};
}


#endif //ATTACK_LINKSTATEUPDATEPACKET_H
