//
// Created by rewbycraft on 2/22/19.
//

#ifndef ATTACK_OSPFV3_H
#define ATTACK_OSPFV3_H

#include <tins/tins.h>

using namespace Tins;

namespace pdu {
	class OSPFv3 : public Tins::PDU {
	public:
		static const PDU::PDUType pdu_flag;
		
		OSPFv3(const uint8_t* data, uint32_t sz);
		
		OSPFv3();
		
		OSPFv3(int version, int type, const IPv4Address &router_id, const IPv4Address &area_id, int instance_id,
		       uint8_t reserved, uint16_t checksum);
		
		uint32_t header_size() const override;
		
		PDU *clone() const override;
		
		PDUType pdu_type() const override;
		
		int getVersion() const;
		
		void setVersion(int version);
		
		int getType() const;
		
		void setType(int type);
		
		IPv4Address getRouterId() const;
		
		void setRouterId(IPv4Address router_id);
		
		IPv4Address getAreaId() const;
		
		void setAreaId(IPv4Address area_id);
		
		int getInstanceId() const;
		
		void setInstanceId(int instance_id);
		
		uint8_t getReserved() const;
		
		void setReserved(uint8_t reserved);
		
		uint16_t getChecksum() const;
		
		void setChecksum(uint16_t checksum);
	
	protected:
		void write_serialization(uint8_t *buffer, uint32_t total_sz) override;
	private:
		int version;
		int type;
		IPv4Address router_id;
		IPv4Address area_id;
		int instance_id;
		uint8_t reserved;
		uint16_t checksum;
		
		typedef struct {
			uint8_t version;
			uint8_t type;
			uint16_t packet_length;
			uint32_t router_id;
			uint32_t area_id;
			uint16_t checksum;
			uint8_t instance_id;
			uint8_t reserved;
		} OSPFv3Header;
		
	};
}


#endif //ATTACK_OSPFV3_H
