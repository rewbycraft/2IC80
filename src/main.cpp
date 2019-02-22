#include <iostream>
#include "pdus/OSPFv3.h"
#include <tins/tins.h>

using namespace Tins;

bool processPacket(const PDU &pdu) {
	const IPv6 &ip = pdu.rfind_pdu<IPv6>();
	const auto &ospf = ip.rfind_pdu<pdu::OSPFv3>();
	std::cout << "IP: " << ip.src_addr() << " -> " << ip.dst_addr() << std::endl;
	std::cout << "OSPF: version=" << ospf.getVersion() << " router_id=" << ospf.getRouterId() << std::endl;
	return true;
}

int main(int argc, char* argv[])
{
	Allocators::register_allocator<IPv6, pdu::OSPFv3>(0x59);
	FileSniffer sniffer("../pcaps/c2-d1.pcap");
	sniffer.sniff_loop(processPacket);
	return 0;
}
