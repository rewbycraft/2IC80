#include <utility>

//
// Created by rewbycraft on 3/25/19.
//
#include "statemachine.h"
#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include "spdlog/spdlog.h"
#include "parser/ospf/HelloPacket.h"

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
			     const std::vector<std::tuple<uint32_t, uint32_t, uint16_t>> &targets)
				: self(self), targets(targets) {}
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
			
			std::shared_ptr<parser::HelloPacket> neighborhello;
			std::shared_ptr<parser::OSPFv3Packet> neighbor;
			
			DiscoverNeighbor() {
				logger->info("Waiting for neighbor...");
			}
			
			sc::result react( const event::Packet &packet) {
				if (packet.packet->getHeader().type != parser::OSPFv3Packet::HELLO)
					return forward_event();
				
				if (packet.packet->getHeader().router_id == context<Machine>().self)
					return forward_event();
				
				neighborhello = std::dynamic_pointer_cast<parser::HelloPacket>(packet.packet->getSubpacket());
				neighbor = packet.packet;
				
				logger->info("Found neighbor {}.", Tins::IPv4Address(parser::byteswap<uint32_t>(neighbor->getHeader().router_id)).to_string());
				
				return transit<DatabaseTransfer>();
			}
		};
		
		struct DatabaseTransfer : sc::simple_state<DatabaseTransfer, Machine, DDCollect> {
			DatabaseTransfer() {
				logger->info("Starting database transfer...");
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