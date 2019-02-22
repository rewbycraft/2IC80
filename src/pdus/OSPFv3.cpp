//
// Created by rewbycraft on 2/22/19.
//

#include "OSPFv3.h"
#include <exception>

using namespace Tins;

const PDU::PDUType pdu::OSPFv3::pdu_flag = PDU::USER_DEFINED_PDU;

uint32_t pdu::OSPFv3::header_size() const {
	return sizeof(OSPFv3Header);
}

Tins::PDU *pdu::OSPFv3::clone() const {
	return new OSPFv3(version, type, router_id, area_id, instance_id, reserved, checksum);
}

Tins::PDU::PDUType pdu::OSPFv3::pdu_type() const {
	return USER_DEFINED_PDU;
}

void pdu::OSPFv3::write_serialization(uint8_t *buffer, uint32_t total_sz) {
	if (total_sz < sizeof(OSPFv3Header))
	{
		throw "Header too large.";
	}
	
	OSPFv3Header header;
	header.version = static_cast<uint8_t>(this->version);
	header.type = static_cast<uint8_t>(this->type);
	header.router_id = this->router_id;
	header.area_id = this->area_id;
	header.instance_id = static_cast<uint8_t>(this->instance_id);
	header.reserved = this->reserved;
	header.checksum = this->checksum;
	
	memcpy(buffer, &header, sizeof(OSPFv3Header));
}

pdu::OSPFv3::OSPFv3(const uint8_t *data, uint32_t sz) {
	OSPFv3Header header;
	
	if (sz < header_size())
	{
		throw "Header too large.";
	}
	
	memcpy(&header, data, sizeof(OSPFv3Header));
	
	if (header.version != 3)
	{
		throw "Non OSPFv3 OSPF packet.";
	}
	
	this->version = header.version;
	this->type = header.type;
	this->router_id = IPv4Address(header.router_id);
	this->area_id = IPv4Address(header.area_id);
	this->instance_id = header.instance_id;
	this->reserved = header.reserved;
	this->checksum = header.checksum;
}

pdu::OSPFv3::OSPFv3() : PDU() {

}

int pdu::OSPFv3::getVersion() const {
	return version;
}

void pdu::OSPFv3::setVersion(int version) {
	OSPFv3::version = version;
}

int pdu::OSPFv3::getType() const {
	return type;
}

void pdu::OSPFv3::setType(int type) {
	OSPFv3::type = type;
}

IPv4Address pdu::OSPFv3::getRouterId() const {
	return router_id;
}

void pdu::OSPFv3::setRouterId(IPv4Address router_id) {
	OSPFv3::router_id = router_id;
}

IPv4Address pdu::OSPFv3::getAreaId() const {
	return area_id;
}

void pdu::OSPFv3::setAreaId(IPv4Address area_id) {
	OSPFv3::area_id = area_id;
}

int pdu::OSPFv3::getInstanceId() const {
	return instance_id;
}

void pdu::OSPFv3::setInstanceId(int instance_id) {
	OSPFv3::instance_id = instance_id;
}

uint8_t pdu::OSPFv3::getReserved() const {
	return reserved;
}

void pdu::OSPFv3::setReserved(uint8_t reserved) {
	OSPFv3::reserved = reserved;
}

pdu::OSPFv3::OSPFv3(int version, int type, const IPv4Address &router_id, const IPv4Address &area_id, int instance_id,
                    uint8_t reserved, uint16_t checksum) : version(version), type(type), router_id(router_id), area_id(area_id),
                                        instance_id(instance_id), reserved(reserved), checksum(checksum) {}

uint16_t pdu::OSPFv3::getChecksum() const {
	return checksum;
}

void pdu::OSPFv3::setChecksum(uint16_t checksum) {
	OSPFv3::checksum = checksum;
}
