//
// Created by rewbycraft on 2/25/19.
//

#include <sstream>
#include "Packet.h"

using namespace parser;


Packet::Packet() { }

Packet::Packet(const bytevector& data) { }

Packet::~Packet() { }

const std::string Packet::toString() const {
	std::stringstream ss;
	
	toString([&](const std::string &str) {
		ss << str << std::endl;
	});
	
	return ss.str();
}
