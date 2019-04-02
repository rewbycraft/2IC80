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
		
		explicit DatabaseDescriptionPacket(const bytevector &data);
		
		const bytevector serialize() const override;

		/**
		 * @return the header of this packet.
		 */
		const Header &getHeader() const;

		/**
		 * Sets the header of this packet.
		 *
		 * @param header the new header of this packet.
		 */
		void setHeader(const Header &header);

		/**
		 * @return a vector containing pointers to the LSAs of this packet.
		 */
		const std::vector<std::shared_ptr<LSAPacket>> &getLsas() const;

		/**
		 * Sets the LSAs of this packet.
		 *
		 * @param lsas the vector containing the pointers to the new LSAs.
		 */
		void setLsas(const std::vector<std::shared_ptr<LSAPacket>> &lsas);

		void toString(const std::function<void(const std::string &)> &printer) const override;
		
		void updateValues() override;
	};
}

BOOST_FUSION_ADAPT_STRUCT(parser::DatabaseDescriptionPacket::Header, (uint32_t, options), (uint16_t, interface_mtu), (uint16_t, database_options), (uint32_t, dd_seq))


#endif //ATTACK_DATABASEDESCRIPTIONPACKET_H
