//
// Created by rewbycraft on 3/24/19.
//

#ifndef ATTACK_TINSHELPER_H
#define ATTACK_TINSHELPER_H

#include <tins/tins.h>
#include "int128.h"

namespace tinshelper {
	const uint128_t tins_to_raw(const Tins::IPv6Address& addr);
	const Tins::IPv6Address raw_to_tins(uint128_t addr);
}

#endif //ATTACK_TINSHELPER_H
