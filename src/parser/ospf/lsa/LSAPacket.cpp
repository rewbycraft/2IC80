//
// Created by rewbycraft on 2/25/19.
//

#include <tins/ip_address.h>
#include "LSAPacket.h"
#include "../../internal.h"
#include "RouterLSAPacket.h"
#include "../../../util.h"

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

void parser::LSAPacket::toString(const std::function<void(const std::string &)> &printer) const {
	printer("== LSA Packet Header ==");
	printer("Age: " + std::to_string(header.age));
	printer("Options: " + util::to_bin_string(header._options));
	printer("ID: " + std::to_string(header.id));
	printer("Advertising Router: " + Tins::IPv4Address(byteswap(header.advertising_router)).to_string());
	printer("Seq: " + std::to_string(header.seq));
	printer("Checksum: " + util::to_hex_string(header.checksum));
	printer("Length: " + std::to_string(header.length));
	if (subpacket) {
		printer("Subpacket:");
		subpacket->toString(util::prepend_printer(printer));
	}
}
