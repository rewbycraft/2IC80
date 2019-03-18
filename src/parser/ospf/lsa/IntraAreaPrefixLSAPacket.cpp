//
// Created by rewbycraft on 3/16/19.
//

#include "IntraAreaPrefixLSAPacket.h"

parser::IntraAreaPrefixLSAPacket::IntraAreaPrefixLSAPacket() {}

parser::IntraAreaPrefixLSAPacket::IntraAreaPrefixLSAPacket(const parser::bytevector &data) : Packet(data) {
	parser::bytevector remainder = parser::deserializeObject(header, data);
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

const parser::bytevector parser::IntraAreaPrefixLSAPacket::serialize() const {
	parser::bytevector raw;
	parser::serializeObject(raw, header);
	
	for (auto& prefix : prefixes) {
		auto p = prefix->serialize();
		raw.insert(raw.end(), p.begin(), p.end());
	}
	
	return raw;
}

void parser::IntraAreaPrefixLSAPacket::toString(const std::function<void(const std::string &)> &printer) const {

}

const parser::IntraAreaPrefixLSAPacket::Header &parser::IntraAreaPrefixLSAPacket::getHeader() const {
	return header;
}

void parser::IntraAreaPrefixLSAPacket::setHeader(const parser::IntraAreaPrefixLSAPacket::Header &header) {
	IntraAreaPrefixLSAPacket::header = header;
}

const std::vector<std::shared_ptr<parser::PrefixLSAPacket>> &parser::IntraAreaPrefixLSAPacket::getPrefixes() const {
	return prefixes;
}

void parser::IntraAreaPrefixLSAPacket::setPrefixes(const std::vector<std::shared_ptr<parser::PrefixLSAPacket>> &prefixes) {
	IntraAreaPrefixLSAPacket::prefixes = prefixes;
}
