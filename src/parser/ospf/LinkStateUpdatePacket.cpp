//
// Created by rewbycraft on 2/25/19.
//

#include "LinkStateUpdatePacket.h"
#include "../internal.h"

parser::LinkStateUpdatePacket::LinkStateUpdatePacket() : Packet() {

}

parser::LinkStateUpdatePacket::LinkStateUpdatePacket(const parser::bytevector &data) : Packet(data) {
	uint32_t numLsas;
	parser::bytevector remainder = parser::deserializeObject(numLsas, data);
	parser::byteswap(numLsas);
	
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
