//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_LINKSTATEREQUESTPACKET_H
#define ATTACK_LINKSTATEREQUESTPACKET_H

#include "../Packet.h"
#include "lsa/LSAPacket.h"


namespace parser {
	class LinkStateRequestPacket : public Packet {
	public:
		typedef struct {
			uint16_t empty;
			uint16_t options;
			uint32_t id;
			uint32_t advertising_router;
		} RequestedLSA;

	private:
		std::vector<RequestedLSA> requestedLsas;

	public:
		LinkStateRequestPacket();
		
		explicit LinkStateRequestPacket(const bytevector &data);
		
		const bytevector serialize() const override;

		/**
		 * @return a vector containing the requested LSAs.
		 */
		const std::vector<RequestedLSA> &getRequestedLsas() const;

		/**
		 * Sets the requested LSAs of this packet.
		 *
		 * @param requestedLsas the vector containing the new requested LSAs.
		 */
		void setRequestedLsas(const std::vector<RequestedLSA> &requestedLsas);

		void toString(const std::function<void(const std::string &)> &printer) const override;
		
		void updateValues() override;
	};
}

BOOST_FUSION_ADAPT_STRUCT(parser::LinkStateRequestPacket::RequestedLSA, (uint16_t, empty), (uint16_t, options), (uint32_t, id), (uint32_t, advertising_router))

#endif //ATTACK_LINKSTATEREQUESTPACKET_H
