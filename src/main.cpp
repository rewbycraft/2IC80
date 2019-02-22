#include <iostream>
#include <tins/tins.h>

using namespace Tins;

bool processPacket(const PDU &) {
	//Some thing here
	return true;
}

int main(int argc, char* argv[])
{
	FileSniffer sniffer("../pcaps/c2-d1.pcap");
	sniffer.sniff_loop(processPacket);
	return 0;
}
