//
// Created by rewbycraft on 2/25/19.
//

#include "LSAPacket.h"
#include "../../internal.h"
#include "RouterLSAPacket.h"

parser::LSAPacket::LSAPacket() : Packet() {

}

parser::LSAPacket::LSAPacket(const parser::bytevector &data) : Packet(data) {
	const parser::bytevector remainder = parser::deserializeObject(header, data);
	
	if (!remainder.empty()) {
		switch (header.getFunction()) {
			case ROUTER_LSA:
				subpacket = std::make_shared<parser::RouterLSAPacket>(remainder);
				break;
			case NETWORK_LSA:
				throw parser::MalformedPacketException("Unimplemented LSA type.");
				break;
			case INTER_AREA_PREFIX_LSA:
				throw parser::MalformedPacketException("Unimplemented LSA type.");
				break;
			case INTER_AREA_ROUTER_LSA:
				throw parser::MalformedPacketException("Unimplemented LSA type.");
				break;
			case AS_EXTERNAL_LSA:
				throw parser::MalformedPacketException("Unimplemented LSA type.");
				break;
			case GROUP_MEMBERSHIP_LSA:
				throw parser::MalformedPacketException("Unimplemented LSA type.");
				break;
			case NSSA_LSA:
				throw parser::MalformedPacketException("Unimplemented LSA type.");
				break;
			case LINK_LSA:
				throw parser::MalformedPacketException("Unimplemented LSA type.");
				break;
			case INTRA_AREA_PREFIX_LSA:
				throw parser::MalformedPacketException("Unimplemented LSA type.");
				break;
			default:
				throw parser::MalformedPacketException("Invalid LSA type.");
		}
	} else {
		subpacket = nullptr;
	}
}

const parser::bytevector parser::LSAPacket::serialize() const {
	parser::bytevector result;
	serializeObject(result, header);

	if (subpacket) {
		auto serializedSubpacket = subpacket->serialize();
		result.insert(result.end(), serializedSubpacket.begin(), serializedSubpacket.end());
	}
	return result;
}

const parser::LSAPacket::Header &parser::LSAPacket::getHeader() const {
	return header;
}

void parser::LSAPacket::setHeader(const parser::LSAPacket::Header &header) {
	LSAPacket::header = header;
}

const std::shared_ptr<parser::Packet> &parser::LSAPacket::getSubpacket() const {
	return subpacket;
}

void parser::LSAPacket::setSubpacket(const std::shared_ptr<parser::Packet> &subpacket) {
	LSAPacket::subpacket = subpacket;
}
