//
// Created by rewbycraft on 3/15/19.
//

#include <iostream>
#include <tins/tins.h>
#include <vector>
#include <cstdint>
#include "../src/util.h"
#include "../src/int128.h"

using namespace Tins;

std::vector<std::tuple<std::vector<std::uint8_t>, uint128_t, uint128_t>> packets;

const uint128_t tins_to_raw(const Tins::IPv6Address& addr) {
	uint128_t v = 0;
	static_assert(sizeof(uint128_t) == Tins::IPv6Address::address_size, "Not enough space.");
	Tins::IPv6Address::const_iterator iter = addr.begin();
	for (; iter != addr.end(); ++iter) {
		v <<= 8;
		v |= *iter;
	}
	return v;
}

bool processPacket(const PDU &pdu) {
	const auto &ip = pdu.rfind_pdu<IPv6>();
	
	if (ip.next_header() == 89) {
		//Is OSPF packet.
		const auto &payload = ip.rfind_pdu<RawPDU>();
		packets.emplace_back(payload.payload(), tins_to_raw(ip.src_addr()), tins_to_raw(ip.dst_addr()));
	}
	
	return true;
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <pcap file name> " << std::endl;
	}
	FileSniffer sniffer(argv[1]);
	sniffer.sniff_loop(processPacket);
	
	int c = 1;
	for (auto& t : packets) {
		auto packet = std::get<0>(t);
		auto src = std::get<1>(t);
		auto dst = std::get<1>(t);
		std::cout << "TEST(ParserTest, ReconstructTest" << c++ << ") {" << std::endl;
		std::cout << "\tparser::bytevector data = {";
		for (size_t i = 0; i < packet.size(); i++) {
			std::cout << util::to_hex_string(packet[i]);
			if (i < packet.size()-1) {
				std::cout << ", ";
			}
		}
		std::cout << "};" << std::endl;
		std::cout << "\tparser::OSPFv3Packet packet(data);" << std::endl;
		std::cout << "\tpacket.setSource(std::make_uint128(" << std::uint128_high(src) << "u," << std::uint128_low(src) << "u));" << std::endl;
		std::cout << "\tpacket.setDest(std::make_uint128(" << std::uint128_high(dst) << "u," << std::uint128_low(dst) << "u));" << std::endl;
		std::cout << "\tpacket.updateValues();" << std::endl;
		std::cout << "\tauto newData = packet.serialize();" << std::endl;
		std::cout << "\tEXPECT_EQ(data, newData);" << std::endl;
		std::cout << "}" << std::endl << std::endl;
	}
	
	return 0;
}
