//
// Created by rewbycraft on 2/25/19.
//

#include <boost/shared_ptr.hpp>
#include "DatabaseDescriptionPacket.h"
#include "../../util.h"

parser::DatabaseDescriptionPacket::DatabaseDescriptionPacket() : Packet() {

}

parser::DatabaseDescriptionPacket::DatabaseDescriptionPacket(const parser::bytevector &data) : Packet(data) {
	auto remainder = parser::deserializeObject(header, data);
	while (!remainder.empty()) {
		std::shared_ptr<LSAPacket> p = std::make_shared<LSAPacket>(
			parser::bytevector(remainder.begin(), remainder.begin() + sizeof(parser::LSAPacket::Header)));
		remainder = parser::bytevector(remainder.begin() + sizeof(parser::LSAPacket::Header), remainder.end());
		lsas.push_back(p);
	}
}

const parser::bytevector parser::DatabaseDescriptionPacket::serialize() const {
	parser::bytevector result;
	serializeObject(result, header);
	for (auto &lsa : lsas) {
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

void parser::DatabaseDescriptionPacket::toString(const std::function<void(const std::string &)> &printer) const {
	printer("== Database Description Packet ==");
	printer("Options: " + util::to_bin_string(header.options));
	printer("Interface MTU: " + std::to_string(header.interface_mtu));
	printer("Database Options: " + util::to_bin_string(header.database_options));
	printer("Database Description SEQ: " + std::to_string(header.dd_seq));
	printer("#LSAs: " + std::to_string(lsas.size()));
	int i = 0;
	for (auto &lsa : lsas) {
		printer("LSA " + std::to_string(i++) + ":");
		lsa->toString(util::prepend_printer(printer));
	}
}

void parser::DatabaseDescriptionPacket::updateValues() {

}
