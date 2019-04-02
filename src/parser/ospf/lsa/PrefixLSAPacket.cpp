//
// Created by rewbycraft on 2/25/19.
//

#include "PrefixLSAPacket.h"
#include <cmath>
#include <tins/ipv6_address.h>
#include "../../../util.h"


parser::PrefixLSAPacket::PrefixLSAPacket() : Packet() { }

parser::PrefixLSAPacket::PrefixLSAPacket(const parser::bytevector &data) : Packet(data) {
	parser::bytevector copy = data;
	while (copy.size() < 20) {
		copy.push_back(0);
	}
	parser::deserializeObject(header, copy);
	
	//Now we get rid of the irrelevant bits from the ip.
	header.address &= (~generateMask<uint128_t>(128UL - header.length));
}

const parser::bytevector parser::PrefixLSAPacket::serialize() const {
	Header copyHeader = header;
	//Now we get rid of the irrelevant bits from the ip.
	copyHeader.address &= (~generateMask<uint128_t>(128UL - copyHeader.length));
	
	parser::bytevector raw;
	parser::serializeObject(raw, copyHeader);
	
	size_t i = 0;
	if (copyHeader.length > 0)
		i += 4;
	if (copyHeader.length > 32)
		i += 4;
	if (copyHeader.length > 64)
		i += 4;
	if (copyHeader.length > 96)
		i += 4;
	
	return parser::bytevector(raw.begin(), raw.begin() + 4 + i);
}

const parser::PrefixLSAPacket::Header &parser::PrefixLSAPacket::getHeader() const {
	return header;
}

void parser::PrefixLSAPacket::setHeader(const parser::PrefixLSAPacket::Header &header) {
	PrefixLSAPacket::header = header;
}

void parser::PrefixLSAPacket::toString(const std::function<void(const std::string &)> &printer) const {
	printer("== Prefix Header ==");
	printer("Length: " + std::to_string(header.length));
	printer("Options: " + util::to_bin_string(header.options));
	printer("Special: " + util::to_bin_string(header.special));
	printer("Address: " + util::to_hex_string(header.address));
}

void parser::PrefixLSAPacket::updateValues() { }

std::pair<std::shared_ptr<parser::PrefixLSAPacket>, const parser::bytevector>
extractPrefix(const parser::bytevector &input) {
	std::shared_ptr<parser::PrefixLSAPacket> packet = std::make_shared<parser::PrefixLSAPacket>(input);
	return {packet, parser::bytevector(input.begin() + packet->serialize().size(), input.end())};
}

std::vector<std::size_t> parser::PrefixLSAPacket::getEmptyByteIndices() {
	return std::vector<std::size_t>{};
}
