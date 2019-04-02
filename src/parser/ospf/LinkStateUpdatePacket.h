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
		
		explicit LinkStateUpdatePacket(const bytevector &data);
		
		const bytevector serialize() const override;

		/**
		 * @return vector containing pointers to the LSAs of this packet.
		 */
		const std::vector<std::shared_ptr<LSAPacket>> &getLsas() const;

		/**
		 * Sets the LSAs of this packet.
		 *
		 * @param lsas vector containing pointers to the new LSAs.
		 */
		void setLsas(const std::vector<std::shared_ptr<LSAPacket>> &lsas);
		
		void toString(const std::function<void(const std::string &)> &printer) const override;
		
		void updateValues() override;
	};
}


#endif //ATTACK_LINKSTATEUPDATEPACKET_H
