//
// Created by rewbycraft on 2/25/19.
//

#include <tins/ip_address.h>
#include "RouterLSAPacket.h"
#include "../../internal.h"
#include "../../../util.h"

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

void parser::RouterLSAPacket::toString(const std::function<void(const std::string &)> &printer) const {
	printer("== Router LSA Header ==");
	printer("Options: " + util::to_bin_string(header.options));
	printer("#interfaces: " + std::to_string(interfaces.size()));
	int i = 0;
	for (auto& interface : interfaces) {
		printer("Interface #" + std::to_string(i++) + ":");
		printer("| Type: " + std::to_string(interface.type));
		printer("| Unused: " + util::to_hex_string(interface.unused));
		printer("| Metric: " + std::to_string(interface.metric));
		printer("| Interface ID: " + std::to_string(interface.interface_id));
		printer("| Neighbor Interface ID: " + std::to_string(interface.neighbor_interface_id));
		printer("| Neighbor Router ID: " + Tins::IPv4Address(byteswap(interface.neighbor_router_id)).to_string());
	}
}

void parser::RouterLSAPacket::updateValues() {

}

std::vector<std::size_t> parser::RouterLSAPacket::getEmptyByteIndices() {
	return std::vector<std::size_t>{1, 5};
}
