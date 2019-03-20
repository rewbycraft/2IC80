#include <iostream>
#include "pdus/OSPFv3.h"
#include "parser/ospf/HelloPacket.h"
#include "parser/ospf/LinkStateUpdatePacket.h"
#include "parser/ospf/LinkStateAcknowledgementPacket.h"
#include <tins/tins.h>

using namespace Tins;

bool processPacket(const PDU &pdu) {
	const IPv6 &ip = pdu.rfind_pdu<IPv6>();
	auto ospf = ip.rfind_pdu<pdu::OSPFv3>();
	std::cout << "IP: " << ip.src_addr() << " -> " << ip.dst_addr() << std::endl;
	
	ospf.updateValues(ip);
	
	std::cout << ospf.getPacket().Packet::toString() << std::endl;
	
	std::cout << "Re-encoded packet: " << std::hex;
	for (auto& i : ospf.getPacket().serialize())
		std::cout << int(i);
	std::cout << std::dec << std::endl;
	
	return true;
}

int attack_main(int argc, char* argv[])
{
	Allocators::register_allocator<IPv6, pdu::OSPFv3>(0x59);
	FileSniffer sniffer("../pcaps/start.pcap");
	sniffer.sniff_loop(processPacket);
	return 0;
}
