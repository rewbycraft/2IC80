//
// Created by rewbycraft on 3/16/19.
//

#include <tins/ip_address.h>
#include "IntraAreaPrefixLSAPacket.h"
#include "../../../util.h"
#include <vector>


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
	printer("== Intra Area Prefix LSA Header ==");
	printer("#prefixes: " + std::to_string(header.num_prefixes));
	printer("Ref LS type: " + std::to_string(header.referenced_lsa_type));
	printer("Ref LS ID: " + std::to_string(header.referenced_link_state_id));
	printer("Ref Advertising Router: " + Tins::IPv4Address(parser::byteswap(header.referenced_advertising_router)).to_string());
	size_t i = 0;
	for (auto& prefix : prefixes) {
		printer("Prefix #" + std::to_string(i++) + ":");
		prefix->toString(util::prepend_printer(printer));
	}
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

void parser::IntraAreaPrefixLSAPacket::updateValues() {
	for (auto& packet : prefixes) {
		packet->updateValues();
	}
}

std::vector<std::size_t> parser::IntraAreaPrefixLSAPacket::getEmptyByteIndices() {
	std::vector<std::size_t> indices;

	int begin = 12;
	for (auto& prefix : prefixes) {
		std::vector<std::size_t> childIndices = prefix->getEmptyByteIndices();
		for (auto j : childIndices) {
			indices.push_back(begin + j);
		}
		begin += prefix->serialize().size();
	}
	return indices;
}
