//
// Created by rewbycraft on 3/18/19.
//

#include "LinkLSAPacket.h"
#include "../../../util.h"

parser::LinkLSAPacket::LinkLSAPacket() : Packet() {

}

parser::LinkLSAPacket::LinkLSAPacket(const parser::bytevector &data) : Packet(data) {
	auto remainder = parser::deserializeObject(header, data);
	
	size_t i = 0;
	while (!remainder.empty()) {
		const auto prefix = std::make_shared<parser::PrefixLSAPacket>(remainder);
		prefixes.push_back(prefix);
		remainder.erase(remainder.begin(), remainder.begin() + prefix->serialize().size());
		
		if ((i++) > header.num_prefixes)
			throw MalformedPacketException("Too many prefixes.");
	}
	
	if (prefixes.size() !=header.num_prefixes)
		throw MalformedPacketException("Wrong amount of prefixes.");
}

const parser::bytevector parser::LinkLSAPacket::serialize() const {
	parser::bytevector raw;
	parser::serializeObject(raw, header);
	
	for (auto& prefix : prefixes) {
		auto p = prefix->serialize();
		raw.insert(raw.end(), p.begin(), p.end());
	}
	
	return raw;
}

void parser::LinkLSAPacket::toString(const std::function<void(const std::string &)> &printer) const {
	printer("== Link LSA Header ==");
	printer("#prefixes: " + std::to_string(header.num_prefixes));
	printer("Router priority: " + std::to_string((uint8_t)(header._options >> 24)));
	printer("Options: " + util::to_bin_string(header._options & 0xFFFFFF));
	printer("LL Addr: " + util::to_hex_string(header.lladdr));
	size_t i = 0;
	for (auto& prefix : prefixes) {
		printer("Prefix #" + std::to_string(i++) + ":");
		prefix->toString(util::prepend_printer(printer));
	}
}

const parser::LinkLSAPacket::Header &parser::LinkLSAPacket::getHeader() const {
	return header;
}

void parser::LinkLSAPacket::setHeader(const parser::LinkLSAPacket::Header &header) {
	LinkLSAPacket::header = header;
}

const std::vector<std::shared_ptr<parser::PrefixLSAPacket>> &parser::LinkLSAPacket::getPrefixes() const {
	return prefixes;
}

void parser::LinkLSAPacket::setPrefixes(const std::vector<std::shared_ptr<parser::PrefixLSAPacket>> &prefixes) {
	LinkLSAPacket::prefixes = prefixes;
}

void parser::LinkLSAPacket::updateValues() {
	for (auto& prefix: prefixes) {
		prefix->updateValues();
	}
}
