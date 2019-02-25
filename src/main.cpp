#include <iostream>
#include "pdus/OSPFv3.h"
#include "parser/HelloPacket.h"
#include <tins/tins.h>

using namespace Tins;

bool processPacket(const PDU &pdu) {
	const IPv6 &ip = pdu.rfind_pdu<IPv6>();
	const auto &ospf = ip.rfind_pdu<pdu::OSPFv3>();
	std::cout << "IP: " << ip.src_addr() << " -> " << ip.dst_addr() << std::endl;
	std::cout << "OSPF: version=" << int(ospf.getPacket().getHeader().version) << " router_id=" << Tins::IPv4Address(ospf.getPacket().getHeader().router_id) << std::endl;
	if (ospf.getPacket().getHeader().type == parser::OSPFv3Packet::HELLO)
		std::cout << "OSPF Hello: neighbors=" << std::dynamic_pointer_cast<parser::HelloPacket>(ospf.getPacket().getSubpacket())->getNeighbors().size() << std::endl;
	
	std::cout << "Re-encoded packet: " << std::hex;
	for (auto& i : ospf.getPacket().serialize())
		std::cout << int(i);
	std::cout << std::dec << std::endl;
	
	return true;
}

int main(int argc, char* argv[])
{
	Allocators::register_allocator<IPv6, pdu::OSPFv3>(0x59);
	FileSniffer sniffer("../pcaps/c2-d1-c1start.pcap");
	sniffer.sniff_loop(processPacket);
	return 0;
}
