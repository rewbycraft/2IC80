//
// Created by rewbycraft on 3/23/19.
//

#ifndef ATTACK_NETNS_H
#define ATTACK_NETNS_H

#include <string>

namespace netns {
	bool exists(const std::string& ns);
	void enter(const std::string& ns);
};

#endif //ATTACK_NETNS_H
