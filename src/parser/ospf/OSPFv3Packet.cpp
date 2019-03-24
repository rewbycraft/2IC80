//
// Created by rewbycraft on 2/25/19.
//

#include "OSPFv3Packet.h"
#include "../MalformedPacketException.h"
#include "../internal.h"
#include "HelloPacket.h"
#include "DatabaseDescriptionPacket.h"
#include "LinkStateRequestPacket.h"
#include "LinkStateUpdatePacket.h"
#include "LinkStateAcknowledgementPacket.h"
#include "../../util.h"
#include "ospf_checksum.h"
#include "../../tinshelper.h"
#include "../../pdus/OSPFv3.h"

parser::OSPFv3Packet::OSPFv3Packet() : Packet() {

}

parser::OSPFv3Packet::OSPFv3Packet(const parser::bytevector& data) : Packet(data) {
	const parser::bytevector sub = parser::deserializeObject(header, data);
	
	switch(header.type) {
		case HELLO:
			subpacket = std::make_shared<parser::HelloPacket>(sub);
			break;
		case DATABASE_DESCRIPTION:
			subpacket = std::make_shared<parser::DatabaseDescriptionPacket>(sub);
			break;
		case LINK_STATE_REQUEST:
			subpacket = std::make_shared<parser::LinkStateRequestPacket>(sub);
			break;
		case LINK_STATE_UPDATE:
			subpacket = std::make_shared<parser::LinkStateUpdatePacket>(sub);
			break;
		case LINK_STATE_ACK:
			subpacket = std::make_shared<parser::LinkStateAcknowledgementPacket>(sub);
			break;
		default:
			throw parser::MalformedPacketException("Invalid OSPF packet type.");
	}
	
	
}

const parser::bytevector parser::OSPFv3Packet::serialize() const {
	
	//Vector from header
	parser::bytevector result;
	parser::serializeObject(result, header);
	
	//Add subpacket to result.
	if (subpacket) {
		auto subpacketVector = subpacket->serialize();
		result.insert(result.end(), subpacketVector.begin(), subpacketVector.end());
	}

	return result;
}

const parser::OSPFv3Packet::Header &parser::OSPFv3Packet::getHeader() const {
	return header;
}

void parser::OSPFv3Packet::setHeader(const parser::OSPFv3Packet::Header &header) {
	OSPFv3Packet::header = header;
}

const std::shared_ptr<parser::Packet> &parser::OSPFv3Packet::getSubpacket() const {
	return subpacket;
}

void parser::OSPFv3Packet::setSubpacket(const std::shared_ptr<parser::Packet> &subpacket) {
	OSPFv3Packet::subpacket = subpacket;
}

void parser::OSPFv3Packet::recomputeChecksum(const Tins::IPv6 &) {

}

void parser::OSPFv3Packet::toString(const std::function<void(const std::string&)>& printer) const {
	printer("== OSPFv3 Header ==");
	printer("Version: " + std::to_string(header.version));
	printer("Type: " + std::to_string(header.type));
	printer("Length: " + std::to_string(header.packet_length));
	printer("Router ID: " + Tins::IPv4Address(byteswap(header.router_id)).to_string());
	printer("Area ID: " + Tins::IPv4Address(byteswap(header.area_id)).to_string());
	printer("Checksum: " + util::to_hex_string(header.checksum));
	printer("Instance ID: " + std::to_string(header.instance_id));
	printer("Reserved: " + util::to_hex_string(header.reserved));
	if (dest != 0) {
		printer("IPv6 Destination: " + util::to_hex_string(dest));
	}
	if (source != 0) {
		printer("IPv6 Source: " + util::to_hex_string(source));
	}
	
	if (subpacket) {
		printer("Subpacket:");
		subpacket->toString(util::prepend_printer(printer));
	}
}

void parser::OSPFv3Packet::updateValues() {
	header.packet_length = 16;
	if (subpacket) {
		subpacket->updateValues();
		header.packet_length += subpacket->serialize().size();
	}

	if (dest != 0 && source != 0) {
		header.checksum = 0;
		header.checksum = parser::checksum::ospf::calcChecksum(
				parser::byteswap(source),
				parser::byteswap(dest),
				parser::byteswap(std::uint32_t(header.packet_length)),
				serialize());
	}
}

uint128_t parser::OSPFv3Packet::getSource() const {
	return source;
}

void parser::OSPFv3Packet::setSource(uint128_t source) {
	OSPFv3Packet::source = source;
}

uint128_t parser::OSPFv3Packet::getDest() const {
	return dest;
}

void parser::OSPFv3Packet::setDest(uint128_t dest) {
	OSPFv3Packet::dest = dest;
}

void parser::OSPFv3Packet::transmit() const {
	if (dest == 0 || source == 0)
		throw MalformedPacketException("Cannot send packet without destination and source.");
	
	Tins::PacketSender sender;
	Tins::IPv6 pkt = Tins::IPv6(tinshelper::raw_to_tins(dest), tinshelper::raw_to_tins(source)) / pdu::OSPFv3(*this);
	sender.send(pkt);
}
