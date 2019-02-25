//
// Created by rewbycraft on 2/25/19.
//

#include "OSPFv3Packet.h"
#include "../MalformedPacketException.h"
#include "../internal.h"
#include "HelloPacket.h"
#include "DatabaseDescriptionPacket.h"
#include "LinkStateRequestPacket.h"
#include "LinkStateUpdatePacket.h"
#include "LinkStateAcknowledgementPacket.h"

parser::OSPFv3Packet::OSPFv3Packet() : Packet() {

}

parser::OSPFv3Packet::OSPFv3Packet(const parser::bytevector& data) : Packet(data) {
	const parser::bytevector sub = parser::deserializeObject(header, data);
	
	switch(header.type) {
		case HELLO:
			subpacket = std::make_shared<parser::HelloPacket>(sub);
			break;
		case DATABASE_DESCRIPTION:
			subpacket = std::make_shared<parser::DatabaseDescriptionPacket>(sub);
			break;
		case LINK_STATE_REQUEST:
			subpacket = std::make_shared<parser::LinkStateRequestPacket>(sub);
			break;
		case LINK_STATE_UPDATE:
			subpacket = std::make_shared<parser::LinkStateUpdatePacket>(sub);
			break;
		case LINK_STATE_ACK:
			subpacket = std::make_shared<parser::LinkStateAcknowledgementPacket>(sub);
			break;
		default:
			throw parser::MalformedPacketException("Invalid OSPF packet type.");
			break;
	}
	
	
}

const parser::bytevector parser::OSPFv3Packet::serialize() const {
	
	//Vector from header
	parser::bytevector result;
	parser::serializeObject(result, header);
	
	//Add subpacket to result.
	if (subpacket) {
		auto subpacketVector = subpacket->serialize();
		result.insert(result.end(), subpacketVector.begin(), subpacketVector.end());
	}

	return result;
}

const parser::OSPFv3Packet::Header &parser::OSPFv3Packet::getHeader() const {
	return header;
}

void parser::OSPFv3Packet::setHeader(const parser::OSPFv3Packet::Header &header) {
	OSPFv3Packet::header = header;
}

const std::shared_ptr<parser::Packet> &parser::OSPFv3Packet::getSubpacket() const {
	return subpacket;
}

void parser::OSPFv3Packet::setSubpacket(const std::shared_ptr<parser::Packet> &subpacket) {
	OSPFv3Packet::subpacket = subpacket;
}

void parser::OSPFv3Packet::recomputeChecksum(const Tins::IPv6 &) {

}
