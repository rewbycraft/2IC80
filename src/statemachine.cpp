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
#include "parser/ospf/LinkStateRequestPacket.h"
#include "parser/ospf/LinkStateUpdatePacket.h"
#include "tinshelper.h"
#include <queue>

namespace sc = boost::statechart;
namespace mpl = boost::mpl;
using namespace std::chrono_literals;

namespace statemachine {
	std::shared_ptr<spdlog::logger> logger;
	std::mutex eventQueueMutex;
	std::queue<std::shared_ptr<sc::event_base>> eventQueue;
	std::atomic_bool keepGoing = true;
	std::thread eventThread;
	
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
		
		struct Timeout : sc::event<Timeout> {
		};
	}
	
	namespace state {
		struct Init;
		struct DiscoverNeighbor;
		struct DatabaseTransfer;
		struct DDCollect;
		struct LSRCollect;
		struct PerformAttack;
	}
	
	struct Machine : sc::state_machine<Machine, state::Init> {
		uint32_t self;
		// [a, b, metric]
		std::vector<std::tuple<uint32_t, uint32_t, uint16_t>> targets;
		std::vector<std::shared_ptr<parser::LSAPacket>> lsas;
		
		
		std::shared_ptr<parser::OSPFv3Packet> neighbor_hello;
		
		std::chrono::system_clock::time_point timeoutTarget;
		std::thread timeoutThread;
		
		void timer() {
			while (timeoutTarget > std::chrono::system_clock::now()) {
				auto t = std::chrono::system_clock::to_time_t(timeoutTarget);
				std::string ct = std::ctime(&t);
				logger->trace("Waiting for timeout {}.", ct.substr(0, ct.size() - 1));
				std::this_thread::sleep_until(timeoutTarget);
			}
			timeoutThread.detach();
			
			logger->trace("Timeout finished.");
			std::lock_guard<std::mutex> guard(eventQueueMutex);
			eventQueue.push(std::make_shared<event::Timeout>());
		}
		
		void setTimeout(std::chrono::system_clock::duration wait) {
			timeoutTarget = std::chrono::system_clock::now() + wait;
			if (!timeoutThread.joinable()) {
				logger->trace("Starting timeout for {}.",
				              std::chrono::duration_cast<std::chrono::seconds>(wait).count());
				timeoutThread = std::thread(&Machine::timer, this);
			} else {
				logger->trace("Resetting timeout for {}.",
				              std::chrono::duration_cast<std::chrono::seconds>(wait).count());
			}
		}
		
		virtual ~Machine() {
			if (timeoutThread.joinable()) {
				logger->info("Waiting for statemachine timer to expire...");
				timeoutThread.join();
			}
		}
	};
	
	void eventProcessor() {
		Machine machine;
		machine.initiate();
		
		while (keepGoing) {
			{
				std::lock_guard<std::mutex> guard(eventQueueMutex);
				if (!eventQueue.empty()) {
					machine.process_event(*eventQueue.front());
					eventQueue.pop();
				}
			}
			std::this_thread::sleep_for(100ms);
		}
	}
	
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
			
			sc::result react(const event::Packet &packet) {
				if (packet.packet->getHeader().type != parser::OSPFv3Packet::HELLO)
					return forward_event();
				
				//We're not our own neighbor.
				if (packet.packet->getHeader().router_id == context<Machine>().self)
					return forward_event();
				
				//Don't use one of the targets as a neighbor.
				for (auto const&[a, b, metric] : context<Machine>().targets)
					if (a == packet.packet->getHeader().router_id ||
					    b == packet.packet->getHeader().router_id)
						return forward_event();
				
				context<Machine>().neighbor_hello = packet.packet;
				
				logger->info("Found neighbor {}.", Tins::IPv4Address(parser::byteswap<uint32_t>(
					context<Machine>().neighbor_hello->getHeader().router_id)).to_string());
				
				return transit<DatabaseTransfer>();
			}
		};
		
		struct DatabaseTransfer : sc::state<DatabaseTransfer, Machine, DDCollect> {
			std::vector<std::shared_ptr<parser::LSAPacket>> partialLSAs;
			typedef sc::state<DatabaseTransfer, Machine, DDCollect> my_base;
			
			DatabaseTransfer(my_context ctx) : my_base(ctx) {
				partialLSAs.clear();
				context<Machine>().lsas.clear();
			}
		};
		
		struct DDCollect : sc::state<DDCollect, DatabaseTransfer> {
			typedef sc::state<DDCollect, DatabaseTransfer> my_base;
			typedef mpl::list<sc::custom_reaction<event::Packet>, sc::custom_reaction<event::Timeout>> reactions;
			
			DDCollect(my_context ctx) : my_base(ctx) {
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
					dbHdr.database_options = 0b0111;
					dbHdr.dd_seq = 0x12345678;
					dbPkt->setHeader(dbHdr);
				}
				
				uint128_t neigh = context<Machine>().neighbor_hello->getSource();
				ospfPkt->setDest(neigh);
				ospfPkt->setSourceFromDest();
				ospfPkt->setDest(tinshelper::tins_to_raw("ff02::5"));
				ospfPkt->updateValues();
				ospfPkt->transmit();
			}
			
			sc::result react(const event::Packet &packet) {
				if (packet.packet->getHeader().type != parser::OSPFv3Packet::DATABASE_DESCRIPTION) {
					return discard_event();
				}
				
				auto &partialLSAs = context<DatabaseTransfer>().partialLSAs;
				auto &lsas = std::dynamic_pointer_cast<parser::DatabaseDescriptionPacket>(
					packet.packet->getSubpacket())->getLsas();
				partialLSAs.insert(partialLSAs.end(), lsas.begin(), lsas.end());
				logger->trace("Obtained {} lsas.", lsas.size());
				
				context<Machine>().setTimeout(1s);
				return discard_event();
				
			}
			
			sc::result react(const event::Timeout &ignored) {
				if (context<DatabaseTransfer>().partialLSAs.empty()) {
					logger->info("No headers yet. Extending timeout.");
					context<Machine>().setTimeout(1s);
					return discard_event();
				} else {
					logger->info("Collected {} LSA-headers in total.",
					             context<DatabaseTransfer>().partialLSAs.size());
					return transit<LSRCollect>();
				}
			}
		};
		
		struct LSRCollect : sc::state<LSRCollect, DatabaseTransfer> {
			typedef sc::state<LSRCollect, DatabaseTransfer> my_base;
			typedef mpl::list<sc::custom_reaction<event::Packet>, sc::custom_reaction<event::Timeout>> reactions;
			
			LSRCollect(my_context ctx) : my_base(ctx) {
				logger->info("Requesting LSAs...");
				auto ospfPkt = std::make_shared<parser::OSPFv3Packet>();
				
				{
					auto ospfHdr = ospfPkt->getHeader();
					ospfHdr.type = parser::OSPFv3Packet::LINK_STATE_REQUEST;
					ospfHdr.router_id = context<Machine>().self;
					ospfHdr.instance_id = 0;
					ospfHdr.area_id = 0;
					ospfHdr.version = 3;
					ospfPkt->setHeader(ospfHdr);
				}
				
				auto lsrPkt = std::make_shared<parser::LinkStateRequestPacket>();
				ospfPkt->setSubpacket(lsrPkt);
				
				{
					auto lsrHdr = lsrPkt->getRequestedLsas();
					for (auto const &partialLSA : context<DatabaseTransfer>().partialLSAs) {
						parser::LinkStateRequestPacket::RequestedLSA requestedLSA;
						requestedLSA.options = partialLSA->getHeader()._options;
						requestedLSA.id = partialLSA->getHeader().id;
						requestedLSA.advertising_router = partialLSA->getHeader().advertising_router;
						requestedLSA.empty = 0;
						lsrHdr.push_back(requestedLSA);
					}
					lsrPkt->setRequestedLsas(lsrHdr);
				}
				
				uint128_t neigh = context<Machine>().neighbor_hello->getSource();
				ospfPkt->setDest(neigh);
				ospfPkt->setSourceFromDest();
				ospfPkt->setDest(tinshelper::tins_to_raw("ff02::5"));
				ospfPkt->updateValues();
				ospfPkt->transmit();
			}
			
			sc::result react(const event::Packet &packet) {
				if (packet.packet->getHeader().type != parser::OSPFv3Packet::LINK_STATE_UPDATE) {
					return discard_event();
				}
				
				auto &fullLSAs = context<Machine>().lsas;
				auto &lsas = std::dynamic_pointer_cast<parser::LinkStateUpdatePacket>(
					packet.packet->getSubpacket())->getLsas();
				fullLSAs.insert(fullLSAs.end(), lsas.begin(), lsas.end());
				logger->trace("Obtained {} lsas.", lsas.size());
				
				context<Machine>().setTimeout(1s);
				return discard_event();
				
			}
			
			sc::result react(const event::Timeout &ignored) {
				auto &v = context<Machine>().lsas;
				
				auto comp = [](const std::shared_ptr<parser::LSAPacket> &lhs,
				               const std::shared_ptr<parser::LSAPacket> &rhs) {
					return lhs->getHeader().checksum == rhs->getHeader().checksum;
				};
				auto pred = [](const std::shared_ptr<parser::LSAPacket> &lhs,
				               const std::shared_ptr<parser::LSAPacket> &rhs) {
					return lhs->getHeader().checksum < rhs->getHeader().checksum;
				};
				
				std::sort(v.begin(), v.end(), pred);
				auto last = std::unique(v.begin(), v.end(), comp);
				v.erase(last, v.end());
				
				if (v.empty()) {
					logger->info("No LSAs yet. Extending timeout.");
					context<Machine>().setTimeout(1s);
					return discard_event();
				} else {
					logger->info("Collected {} LSAs in total.", v.size());
					return transit<PerformAttack>();
				}
			}
			
		};
		
		struct PerformAttack : sc::state<PerformAttack, Machine> {
			typedef sc::state<PerformAttack, Machine> my_base;
			
			PerformAttack(my_context ctx) : my_base(ctx) {
				logger->info("Performing attack.");
				
				for (auto const&[a, b, metric] : context<Machine>().targets) {
				
				}
			}
		};
	}
	
	
	void onPacket(const std::shared_ptr<parser::OSPFv3Packet> &p) {
		std::lock_guard<std::mutex> guard(eventQueueMutex);
		eventQueue.push(std::make_shared<event::Packet>(p));
	}
	
	void init(const uint32_t &self, const std::vector<std::tuple<uint32_t, uint32_t, uint16_t>> &targets) {
		if (!eventThread.joinable()) {
			logger = spdlog::get("statemachine");
			eventThread = std::thread(eventProcessor);
			std::lock_guard<std::mutex> guard(eventQueueMutex);
			eventQueue.push(std::make_shared<event::Init>(self, targets));
		}
	}
}