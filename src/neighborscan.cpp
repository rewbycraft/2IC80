//
// Created by rewbycraft on 3/27/19.
//

#include <spdlog/spdlog.h>
#include "neighborscan.h"
#include "parser/MalformedPacketException.h"

std::map<std::string, std::map<Tins::IPv6Address, std::vector<std::tuple<Tins::HWAddress<6>,Tins::IPv6Address>>>> neighbors;

void neighborscan::discoverNeighbors() {
	auto logger = spdlog::get("neighbor");
	neighbors.clear();
	Tins::PacketSender sender;
	logger->info("Performing neighbor discovery on all interfaces...");
	size_t count = 0;
	for (auto &_intf : Tins::Utils::network_interfaces()) {
		Tins::NetworkInterface intf(_intf);
		
		if (intf.is_loopback()) {
			continue;
		}
		
		if (!intf.is_up()) {
			continue;
		}
		
		std::map<Tins::IPv6Address, std::vector<std::tuple<Tins::HWAddress<6>,Tins::IPv6Address>>> addrs;
		
		for (auto &addr : intf.ipv6_addresses()) {
			
			auto pkt = Tins::EthernetII(Tins::EthernetII::BROADCAST, intf.hw_address()) /
			           Tins::IPv6(Tins::IPv6Address("ff02::1"), addr.address) /
			           Tins::ICMPv6(Tins::ICMPv6::ECHO_REQUEST);
			
			sender.send(pkt, intf);
			
			Tins::PDU *response;
			std::vector<std::tuple<Tins::HWAddress<6>,Tins::IPv6Address>> neighs;
			while ((response = pkt.recv_response(sender, intf)) != 0) {
				auto e = response->rfind_pdu<Tins::EthernetII>();
				auto ip6 = response->rfind_pdu<Tins::IPv6>();
				neighs.emplace_back(e.src_addr(), ip6.src_addr());
				count++;
				logger->debug("Discovered neighbor {} with MAC {} on interface {} using source address {}.", ip6.src_addr().to_string(), e.src_addr().to_string(), intf.name(), addr.address.to_string());
				delete response;
			}
			
			addrs[addr.address] = neighs;
		}
		
		neighbors[intf.name()] = addrs;
	}
	logger->info("Discovered {} neighbors.", count);
}

Tins::NetworkInterface neighborscan::getInterfaceForNeighbor(Tins::IPv6Address addr) {
	for (auto const& [intf, ips] : neighbors) {
		for (auto const& [src, neighs] : ips) {
			for (auto const& [mac,neigh] : neighs) {
				if (neigh == addr) {
					return intf;
				}
			}
		}
	}
	throw parser::MalformedPacketException("Cannot get interface for non-neighbor.");
}

Tins::IPv6Address neighborscan::getSourceForNeighbor(Tins::IPv6Address addr) {
	for (auto const&[intf, ips] : neighbors) {
		for (auto const&[src, neighs] : ips) {
			for (auto const &[mac,neigh] : neighs) {
				if (addr == neigh) {
					return src;
				}
			}
		}
	}
	throw parser::MalformedPacketException("Cannot get source for non-neighbor.");
}

Tins::HWAddress<6> neighborscan::getMACForNeighbor(Tins::IPv6Address addr) {
	for (auto const& [intf, ips] : neighbors) {
		for (auto const& [src, neighs] : ips) {
			for (auto const& [mac,neigh] : neighs) {
				if (neigh == addr) {
					return mac;
				}
			}
		}
	}
	throw parser::MalformedPacketException("Cannot get MAC for non-neighbor.");
}