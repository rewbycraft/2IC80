//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_DATABASEDESCRIPTIONPACKET_H
#define ATTACK_DATABASEDESCRIPTIONPACKET_H

#include "../Packet.h"
#include "lsa/LSAPacket.h"

namespace parser {
	class DatabaseDescriptionPacket : public Packet {
	public:
		typedef struct {
			uint8_t empty;
			uint32_t options : 24;
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


#endif //ATTACK_DATABASEDESCRIPTIONPACKET_H
