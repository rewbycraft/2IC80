#include <iostream>
#include "pdus/OSPFv3.h"
#include "../spdlog/include/spdlog/sinks/stdout_color_sinks.h"
#include "netns.h"
#include <boost/program_options.hpp>

using namespace Tins;

bool processPacket(const PDU &pdu) {
	const IPv6 &ip = pdu.rfind_pdu<IPv6>();
	auto ospf = ip.rfind_pdu<pdu::OSPFv3>();
	std::cout << "IP: " << ip.src_addr() << " -> " << ip.dst_addr() << std::endl;
	std::cout << ospf.getPacket().Packet::toString() << std::endl;
	
	return true;
}

int attack_main(int argc, char *argv[]) {
	boost::program_options::options_description desc{"Options"};
	desc.add_options()
		("help,h", "Help")
		("netns,n", boost::program_options::value<std::string>(), "Execute in this network namespace.")
		("log,l", boost::program_options::value<typename std::underlying_type<spdlog::level::level_enum>::type>()->default_value(spdlog::level::info), "Loglevel");
	
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);
	
	//Create spdloggers
	for (auto &s : {"main", "netns"}) {
		auto l = spdlog::stdout_color_mt(s);
		l->set_level(static_cast<spdlog::level::level_enum>(vm["log"].as<typename std::underlying_type<spdlog::level::level_enum>::type>()));
	}
	
	auto logger = spdlog::get("main");
	logger->info("Options:");
	if (vm.count("netns"))
		logger->info("  netns: {}", vm["netns"].as<std::string>());
	else
		logger->info("  netns: <none>");
	logger->info("  log level: {}", vm["log"].as<typename std::underlying_type<spdlog::level::level_enum>::type>());
	
	if (vm.count("netns")) {
		netns::enter(vm["netns"].as<std::string>());
	}
	
	logger->info("Setting up sniffer...");
	Allocators::register_allocator<IPv6, pdu::OSPFv3>(0x59);
	Sniffer sniffer("any");
	
	logger->info("Starting sniffer...");
	sniffer.sniff_loop(processPacket);
	return 0;
}
