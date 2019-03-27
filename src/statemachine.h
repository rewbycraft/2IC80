//
// Created by rewbycraft on 3/25/19.
//

#ifndef ATTACK_STATEMACHINE_H
#define ATTACK_STATEMACHINE_H

#include "parser/ospf/OSPFv3Packet.h"

namespace statemachine {
	void onPacket(const std::shared_ptr<parser::OSPFv3Packet> &p);
	void init(const uint32_t &self, const std::vector<std::tuple<uint32_t, uint32_t, uint16_t>> &targets);
}

#endif //ATTACK_STATEMACHINE_H
