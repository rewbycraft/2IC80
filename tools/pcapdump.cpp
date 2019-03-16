//
// Created by rewbycraft on 3/15/19.
//

#include <iostream>
#include <tins/tins.h>
#include <vector>
#include <cstdint>
#include "../src/util.h"

using namespace Tins;

std::vector<std::vector<std::uint8_t>> packets;

bool processPacket(const PDU &pdu) {
	const auto &ip = pdu.rfind_pdu<IPv6>();
	
	if (ip.next_header() == 89) {
		//Is OSPF packet.
		const auto &payload = ip.rfind_pdu<RawPDU>();
		packets.push_back(payload.payload());
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
	for (auto& packet : packets) {
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
		std::cout << "\tauto newData = packet.serialize();" << std::endl;
		std::cout << "\tEXPECT_EQ(data, newData);" << std::endl;
		std::cout << "}" << std::endl << std::endl;
	}
	
	return 0;
}
