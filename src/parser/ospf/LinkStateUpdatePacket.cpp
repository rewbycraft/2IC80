//
// Created by rewbycraft on 2/25/19.
//

#include "LinkStateUpdatePacket.h"
#include "../internal.h"
#include "../../util.h"

parser::LinkStateUpdatePacket::LinkStateUpdatePacket() : Packet() {

}

parser::LinkStateUpdatePacket::LinkStateUpdatePacket(const parser::bytevector &data) : Packet(data) {
	uint32_t numLsas;
	parser::bytevector remainder = parser::deserializeObject(numLsas, data);
	
	while(!remainder.empty()) {
		std::shared_ptr<parser::LSAPacket> lsa = std::make_shared<parser::LSAPacket>(remainder);
		auto s = lsa->serialize().size();
		if (s > remainder.size())
			throw parser::MalformedPacketException("AAAA");
		remainder.erase(remainder.begin(), remainder.begin() + s);
		lsas.push_back(lsa);
	}
	
	if (lsas.size() != numLsas)
		throw parser::MalformedPacketException("Invalid number of LSAs.");
}

const parser::bytevector parser::LinkStateUpdatePacket::serialize() const {
	parser::bytevector result;
	parser::serializeObject(result, static_cast<uint32_t>(lsas.size()));
	for (auto& lsa : lsas) {
		auto serializedLsa = lsa->serialize();
		result.insert(result.end(), serializedLsa.begin(), serializedLsa.end());
	}
	return result;
}

const std::vector<std::shared_ptr<parser::LSAPacket>> &parser::LinkStateUpdatePacket::getLsas() const {
	return lsas;
}

void parser::LinkStateUpdatePacket::setLsas(const std::vector<std::shared_ptr<parser::LSAPacket>> &lsas) {
	LinkStateUpdatePacket::lsas = lsas;
}

void parser::LinkStateUpdatePacket::toString(const std::function<void(const std::string &)> &printer) const {
	printer("== Link State Update Header ==");
	printer("#LSAs: " + std::to_string(lsas.size()));
	int i = 0;
	for (auto& lsa : lsas) {
		printer("LSA " + std::to_string(i++) + ":");
		lsa->toString(util::prepend_printer(printer));
	}
}
