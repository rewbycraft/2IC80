//
// Created by rewbycraft on 2/25/19.
//

#include "PrefixLSAPacket.h"
#include "../../internal.h"
#include <cmath>

parser::PrefixLSAPacket::PrefixLSAPacket() : Packet() {

}

parser::PrefixLSAPacket::PrefixLSAPacket(const parser::bytevector &data) : Packet(data) {
	deserializeObject(header, data);
	
	//Now we get rid of the irrelevant bits from the ip.
	header.address &= (~generateMask<uint128_t>(128UL - header.length));
}

const parser::bytevector parser::PrefixLSAPacket::serialize() const {
	Header copyHeader = header;
	//Now we get rid of the irrelevant bits from the ip.
	copyHeader.address &= (~generateMask<uint128_t>(128UL - copyHeader.length));
	
	parser::bytevector raw;
	serializeObject(raw, copyHeader);
	
	return parser::bytevector(raw.begin(), raw.begin()+4+int(std::max(std::ceil((float)copyHeader.length/32.0),1.0))*4);
}

const parser::PrefixLSAPacket::Header &parser::PrefixLSAPacket::getHeader() const {
	return header;
}

void parser::PrefixLSAPacket::setHeader(const parser::PrefixLSAPacket::Header &header) {
	PrefixLSAPacket::header = header;
}

std::pair<std::shared_ptr<parser::PrefixLSAPacket>, const parser::bytevector> extractPrefix(const parser::bytevector& input) {
	std::shared_ptr<parser::PrefixLSAPacket> packet = std::make_shared<parser::PrefixLSAPacket>(input);
	return {packet, parser::bytevector(input.begin() + packet->serialize().size(), input.end())};
}

