//
// Created by rewbycraft on 2/25/19.
//

#include <tins/ip_address.h>
#include "LinkStateRequestPacket.h"
#include "../internal.h"
#include "../../util.h"

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

void parser::LinkStateRequestPacket::toString(const std::function<void(const std::string &)> &printer) const {
	printer("== Link State Request Header ==");
	printer("#LSAs: " + std::to_string(requestedLsas.size()));
	int i = 0;
	for (auto& lsa : requestedLsas) {
		printer("LSA " + std::to_string(i++) + ":");
		printer("| Empty: " + util::to_hex_string(lsa.empty));
		printer("| Options: " + util::to_bin_string(lsa.options));
		printer("| ID: " + util::to_hex_string(lsa.id));
		printer("| Advertising Router: " + Tins::IPv4Address(byteswap(lsa.advertising_router)).to_string());
	}
}

void parser::LinkStateRequestPacket::updateValues() {

}
