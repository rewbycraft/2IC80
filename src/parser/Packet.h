//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_PACKET_H
#define ATTACK_PACKET_H

#include <vector>
#include <cstdint>
#include "internal.h"
#include <functional>
#include <string>

namespace parser {
	class Packet {
	public:
		Packet();
		
		//Deserialization constructor
		Packet(const bytevector& data);
		
		virtual ~Packet();
		
		virtual const bytevector serialize() const = 0;
		
		virtual void toString(const std::function<void(const std::string&)>& printer) const = 0;
		
		virtual const std::string toString() const;
	};
	
}

#endif //ATTACK_PACKET_H
