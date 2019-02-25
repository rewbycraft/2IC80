//
// Created by rewbycraft on 2/25/19.
//

#include <boost/shared_ptr.hpp>
#include "DatabaseDescriptionPacket.h"
#include "internal.h"

parser::DatabaseDescriptionPacket::DatabaseDescriptionPacket() : Packet() {

}

parser::DatabaseDescriptionPacket::DatabaseDescriptionPacket(const parser::bytevector &data) : Packet(data) {
	auto remainder = parser::deserializeObject(header, data);
	unsigned long i = 0;
	while (remainder.size()-i > 0) {
		std::shared_ptr<LSAPacket> p = std::make_shared<LSAPacket>(parser::bytevector(remainder.begin()+i, remainder.end()));
		i += p->serialize().size();
		lsas.push_back(p);
	}
}

const parser::bytevector parser::DatabaseDescriptionPacket::serialize() const {
	parser::bytevector result;
	serializeObject(result, header);
	for (auto& lsa : lsas) {
		auto serializedLsa = lsa->serialize();
		result.insert(result.end(), serializedLsa.begin(), serializedLsa.end());
	}
	return result;
}

const parser::DatabaseDescriptionPacket::Header &parser::DatabaseDescriptionPacket::getHeader() const {
	return header;
}

void parser::DatabaseDescriptionPacket::setHeader(const parser::DatabaseDescriptionPacket::Header &header) {
	DatabaseDescriptionPacket::header = header;
}

const std::vector<std::shared_ptr<parser::LSAPacket>> &parser::DatabaseDescriptionPacket::getLsas() const {
	return lsas;
}

void parser::DatabaseDescriptionPacket::setLsas(const std::vector<std::shared_ptr<parser::LSAPacket>> &lsas) {
	DatabaseDescriptionPacket::lsas = lsas;
}
