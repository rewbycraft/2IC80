//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_DATABASEDESCRIPTIONPACKET_H
#define ATTACK_DATABASEDESCRIPTIONPACKET_H

#include "../Packet.h"
#include "lsa/LSAPacket.h"
#include <boost/fusion/include/adapt_struct.hpp>

namespace parser {
	class DatabaseDescriptionPacket : public Packet {
	public:
		typedef struct {
			uint32_t options;
			uint16_t interface_mtu;
			uint16_t database_options;
			uint32_t dd_seq;
		} Header;
	private:
		Header header;
		std::vector<std::shared_ptr<parser::LSAPacket>> lsas;
	public:
		DatabaseDescriptionPacket();
		
		DatabaseDescriptionPacket(const bytevector &data);
		
		const bytevector serialize() const override;
		
		const Header &getHeader() const;
		
		void setHeader(const Header &header);
		
		const std::vector<std::shared_ptr<LSAPacket>> &getLsas() const;
		
		void setLsas(const std::vector<std::shared_ptr<LSAPacket>> &lsas);
	};
}

BOOST_FUSION_ADAPT_STRUCT(parser::DatabaseDescriptionPacket::Header, (uint32_t, options), (uint16_t, interface_mtu), (uint16_t, database_options), (uint32_t, dd_seq))


#endif //ATTACK_DATABASEDESCRIPTIONPACKET_H
