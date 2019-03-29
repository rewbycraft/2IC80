//
// Created by rewbycraft on 3/27/19.
//

#ifndef ATTACK_NEIGHBORSCAN_H
#define ATTACK_NEIGHBORSCAN_H

#include <tins/tins.h>

namespace neighborscan {
	Tins::NetworkInterface getInterfaceForNeighbor(Tins::IPv6Address);
	Tins::IPv6Address getSourceForNeighbor(Tins::IPv6Address);
	void discoverNeighbors();
}

#endif //ATTACK_NEIGHBORSCAN_H
