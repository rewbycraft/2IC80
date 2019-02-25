//
// Created by rewbycraft on 2/25/19.
//

#include "LinkStateRequestPacket.h"
#include "../internal.h"

parser::LinkStateRequestPacket::LinkStateRequestPacket() : Packet() {

}

parser::LinkStateRequestPacket::LinkStateRequestPacket(const parser::bytevector &data) : Packet(data) {
	parser::bytevector c(data);
	while (!c.empty()) {
		RequestedLSA r;
		c = parser::deserializeObject(r, c);
		requestedLsas.push_back(r);
	}
}

const parser::bytevector parser::LinkStateRequestPacket::serialize() const {
	parser::bytevector result;
	for (auto& r : requestedLsas)
		serializeObject(result, r);
	return result;
}

const std::vector<parser::LinkStateRequestPacket::RequestedLSA> &
parser::LinkStateRequestPacket::getRequestedLsas() const {
	return requestedLsas;
}

void parser::LinkStateRequestPacket::setRequestedLsas(
	const std::vector<parser::LinkStateRequestPacket::RequestedLSA> &requestedLsas) {
	LinkStateRequestPacket::requestedLsas = requestedLsas;
}
