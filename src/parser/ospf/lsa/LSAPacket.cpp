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
	parser::byteswap(header);

	if (header.length > data.size())
		throw parser::MalformedPacketException("Length mismatch.");
	
	switch (header.function){
		case ROUTER_LSA:
			subpacket = std::make_shared<parser::RouterLSAPacket>(remainder);
			break;
		case NETWORK_LSA:break;
		case INTER_AREA_PREFIX_LSA:break;
		case INTER_AREA_ROUTER_LSA:break;
		case AS_EXTERNAL_LSA:break;
		case GROUP_MEMBERSHIP_LSA:break;
		case NSSA_LSA:break;
		case LINK_LSA:break;
		case INTRA_AREA_PREFIX_LSA:break;
		default:
			throw parser::MalformedPacketException("Invalid LSA type.");
	}
}

const parser::bytevector parser::LSAPacket::serialize() const {
	parser::bytevector result;
	Header header1 = header;
	
	parser::byteswap(header1);
	
	serializeObject(result, header1);

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
