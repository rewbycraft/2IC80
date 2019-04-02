//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_LINKSTATEACKNOWLEDGEMENTPACKET_H
#define ATTACK_LINKSTATEACKNOWLEDGEMENTPACKET_H

#include "lsa/LSAPacket.h"


namespace parser {
	class LinkStateAcknowledgementPacket : public Packet {
	private:
		std::vector<std::shared_ptr<LSAPacket>> lsas;

	public:
		LinkStateAcknowledgementPacket();
		
		explicit LinkStateAcknowledgementPacket(const bytevector &data);
		
		const bytevector serialize() const override;

		/**
		 * @return a vector containing pointers to the LSAs of this packet.
		 */
		const std::vector<std::shared_ptr<LSAPacket>> &getLsas() const;

		/**
		 * Sets the LSAs of this packet.
		 *
		 * @param lsas the vector containing the pointers to the new LSAs of this packet.
		 */
		void setLsas(const std::vector<std::shared_ptr<LSAPacket>> &lsas);

		void toString(const std::function<void(const std::string &)> &printer) const override;
		
		void updateValues() override;
	};
}


#endif //ATTACK_LINKSTATEACKNOWLEDGEMENTPACKET_H
