//
// Created by rewbycraft on 2/25/19.
//

#include "RouterLSAPacket.h"
#include "../../internal.h"

parser::RouterLSAPacket::RouterLSAPacket() : Packet() {

}

parser::RouterLSAPacket::RouterLSAPacket(const parser::bytevector &data) : Packet(data) {
	parser::bytevector remainder = parser::deserializeObject(header, data);
	while (!remainder.empty()) {
		InterfaceHeader header1;
		remainder = deserializeObject(header1, remainder);
		interfaces.push_back(header1);
	}
}

const parser::bytevector parser::RouterLSAPacket::serialize() const {
	parser::bytevector result;
	parser::serializeObject(result, header);
	
	for (auto& header1 : interfaces) {
		parser::serializeObject(result, header1);
	}
	
	return result;
}

const parser::RouterLSAPacket::Header &parser::RouterLSAPacket::getHeader() const {
	return header;
}

void parser::RouterLSAPacket::setHeader(const parser::RouterLSAPacket::Header &header) {
	RouterLSAPacket::header = header;
}

const std::vector<parser::RouterLSAPacket::InterfaceHeader> &
parser::RouterLSAPacket::getInterfaces() const {
	return interfaces;
}

void parser::RouterLSAPacket::setInterfaces(
	const std::vector<parser::RouterLSAPacket::InterfaceHeader> &interfaces) {
	RouterLSAPacket::interfaces = interfaces;
}
