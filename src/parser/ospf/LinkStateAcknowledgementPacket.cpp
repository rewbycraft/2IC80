//
// Created by rewbycraft on 2/25/19.
//

#include "LinkStateAcknowledgementPacket.h"
#include "../MalformedPacketException.h"

parser::LinkStateAcknowledgementPacket::LinkStateAcknowledgementPacket() : Packet() {

}

parser::LinkStateAcknowledgementPacket::LinkStateAcknowledgementPacket(const parser::bytevector &data) : Packet(data) {
	parser::bytevector remainder = data;
	
	while(!remainder.empty()) {
		std::shared_ptr<parser::LSAPacket> lsa = std::make_shared<parser::LSAPacket>();
		auto s = lsa->serialize().size();
		if (s > remainder.size())
			throw parser::MalformedPacketException("AAAA");
		remainder.erase(remainder.begin(), remainder.begin() + s);
		lsas.push_back(lsa);
	}
}

const parser::bytevector parser::LinkStateAcknowledgementPacket::serialize() const {
	parser::bytevector result;
	for (auto& lsa : lsas) {
		auto serializedLsa = lsa->serialize();
		result.insert(result.end(), serializedLsa.begin(), serializedLsa.end());
	}
	return result;
}

const std::vector<std::shared_ptr<parser::LSAPacket>> &parser::LinkStateAcknowledgementPacket::getLsas() const {
	return lsas;
}

void parser::LinkStateAcknowledgementPacket::setLsas(const std::vector<std::shared_ptr<parser::LSAPacket>> &lsas) {
	LinkStateAcknowledgementPacket::lsas = lsas;
}