//
// Created by rewbycraft on 3/18/19.
//

#include "ASExternalLSAPacket.h"
#include "../../../util.h"

parser::ASExternalLSAPacket::ASExternalLSAPacket() : Packet() {

}

parser::ASExternalLSAPacket::ASExternalLSAPacket(const parser::bytevector &data) : Packet(data) {
	auto remainder = parser::deserializeObject(header._options, data);
	
	prefix = std::make_shared<parser::PrefixLSAPacket>(remainder);
	remainder.erase(remainder.begin(), remainder.begin() + prefix->serialize().size());
	
	if ((header._options >> 25) & 0x1) {
		remainder = parser::deserializeObject(header.forwarding_address, remainder);
	}
	
	if ((header._options >> 24) & 0x1) {
		remainder = parser::deserializeObject(header.external_route_tag, remainder);
	}
	
	if (prefix->getHeader().special != 0) {
		remainder = parser::deserializeObject(header.referenced_link_state_id, remainder);
	}
	
}

const parser::bytevector parser::ASExternalLSAPacket::serialize() const {
	parser::bytevector raw;
	parser::serializeObject(raw, header._options);
	
	auto p = prefix->serialize();
	raw.insert(raw.end(), p.begin(), p.end());
	
	if ((header._options >> 25) & 0x1) {
		parser::serializeObject(raw, header.forwarding_address);
	}
	
	if ((header._options >> 24) & 0x1) {
		parser::serializeObject(raw, header.external_route_tag);
	}
	
	if (prefix->getHeader().special != 0) {
		parser::serializeObject(raw, header.referenced_link_state_id);
	}
	
	return raw;
}

void parser::ASExternalLSAPacket::toString(const std::function<void(const std::string &)> &printer) const {
	printer("== AS External LSA Header ==");
	printer("Options: " + util::to_bin_string(header._options));
	printer("External route tag: " + util::to_hex_string(header.external_route_tag));
	printer("Referenced LS id: " + std::to_string(header.referenced_link_state_id));
	printer("Prefix:");
	prefix->toString(util::prepend_printer(printer));

}

const parser::ASExternalLSAPacket::Header &parser::ASExternalLSAPacket::getHeader() const {
	return header;
}

void parser::ASExternalLSAPacket::setHeader(const parser::ASExternalLSAPacket::Header &header) {
	ASExternalLSAPacket::header = header;
}

const std::shared_ptr<parser::PrefixLSAPacket> &parser::ASExternalLSAPacket::getPrefix() const {
	return prefix;
}

void parser::ASExternalLSAPacket::setPrefix(const std::shared_ptr<parser::PrefixLSAPacket> &prefix) {
	ASExternalLSAPacket::prefix = prefix;
}

void parser::ASExternalLSAPacket::updateValues() {
	prefix->updateValues();
}
