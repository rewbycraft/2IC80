#include <utility>

#include <utility>

//
// Created by rewbycraft on 3/25/19.
//
#include "statemachine.h"
#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include "spdlog/spdlog.h"
#include "parser/ospf/HelloPacket.h"
#include "parser/ospf/DatabaseDescriptionPacket.h"

namespace sc = boost::statechart;

namespace statemachine {
	std::shared_ptr<spdlog::logger> logger;
	
	namespace event {
		struct Init : sc::event<Init> {
			const uint32_t self;
			// [a, b, metric]
			const std::vector<std::tuple<uint32_t, uint32_t, uint16_t>> targets;
			
			Init() = delete;
			Init(const uint32_t self,
			     std::vector<std::tuple<uint32_t, uint32_t, uint16_t>> targets)
				: self(self), targets(std::move(targets)) {}
		};
		
		struct Packet : sc::event<Packet> {
			const std::shared_ptr<parser::OSPFv3Packet> packet;
			Packet() = delete;
			
			explicit Packet(std::shared_ptr<parser::OSPFv3Packet> packet) : packet(std::move(packet)) {}
		};
	}
	
	namespace state {
		struct Init;
		struct DiscoverNeighbor;
		struct DatabaseTransfer;
		struct DDCollect;
	}
	
	struct Machine : sc::state_machine<Machine, state::Init> {
		uint32_t self;
		// [a, b, metric]
		std::vector<std::tuple<uint32_t, uint32_t, uint16_t>> targets;
		
		std::shared_ptr<parser::OSPFv3Packet> neighbor_hello;
		
	} machine;
	
	namespace state {
		
		struct Init : sc::simple_state<Init, Machine> {
			
			typedef sc::custom_reaction<event::Init> reactions;
			
			sc::result react(const event::Init &init) {
				context<Machine>().self = init.self;
				context<Machine>().targets = init.targets;
				
				return transit<DiscoverNeighbor>();
			}
		};

		struct DiscoverNeighbor : sc::simple_state<DiscoverNeighbor, Machine> {
			typedef sc::custom_reaction<event::Packet> reactions;
			
			DiscoverNeighbor() {
				logger->info("Waiting for neighbor...");
			}
			
			sc::result react( const event::Packet &packet) {
				if (packet.packet->getHeader().type != parser::OSPFv3Packet::HELLO)
					return forward_event();
				
				//We're not our own neighbor.
				if (packet.packet->getHeader().router_id == context<Machine>().self)
					return forward_event();
				
				//Don't use one of the targets as a neighbor.
				for (auto const& [a, b, metric] : context<Machine>().targets)
					if (a == packet.packet->getHeader().router_id || b == packet.packet->getHeader().router_id)
						return forward_event();
				
				context<Machine>().neighbor_hello = packet.packet;
				
				logger->info("Found neighbor {}.", Tins::IPv4Address(parser::byteswap<uint32_t>(context<Machine>().neighbor_hello->getHeader().router_id)).to_string());
				
				return transit<DatabaseTransfer>();
			}
		};
		
		struct DatabaseTransfer : sc::state<DatabaseTransfer, Machine, DDCollect> {
			typedef sc::state< DatabaseTransfer, Machine, DDCollect > my_base;
			DatabaseTransfer(my_context ctx) : my_base(ctx) {
				logger->info("Starting database transfer...");
				auto ospfPkt = std::make_shared<parser::OSPFv3Packet>();
				
				{
					auto ospfHdr = ospfPkt->getHeader();
					ospfHdr.type = parser::OSPFv3Packet::DATABASE_DESCRIPTION;
					ospfHdr.router_id = context<Machine>().self;
					ospfHdr.instance_id = 0;
					ospfHdr.area_id = 0;
					ospfHdr.version = 3;
					ospfPkt->setHeader(ospfHdr);
				}
				
				auto dbPkt = std::make_shared<parser::DatabaseDescriptionPacket>();
				ospfPkt->setSubpacket(dbPkt);
				
				{
					auto dbHdr = dbPkt->getHeader();
					dbHdr.options = 0x113;
					dbHdr.interface_mtu = 1500;
					dbHdr.database_options = 0x07;
					dbHdr.dd_seq = 0x12345678;
					dbPkt->setHeader(dbHdr);
				}
				
				uint128_t neigh = context<Machine>().neighbor_hello->getSource();
				ospfPkt->setDest(neigh);
				ospfPkt->setSourceFromDest();
				ospfPkt->updateValues();
				ospfPkt->transmit();
			}
		};
		
		struct DDCollect : sc::simple_state<DDCollect, DatabaseTransfer> {
		
		};
	}
	
	

	void onPacket(const std::shared_ptr<parser::OSPFv3Packet> &p) {
		machine.process_event(event::Packet(p));
	}
	
	void init(const uint32_t &self, const std::vector<std::tuple<uint32_t, uint32_t, uint16_t>> &targets) {
		if (machine.terminated()) {
			logger = spdlog::get("statemachine");
			machine.initiate();
			machine.process_event(event::Init(self, targets));
		}
	}
}