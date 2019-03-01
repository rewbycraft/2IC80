//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_PACKET_H
#define ATTACK_PACKET_H

#include <vector>
#include <cstdint>

typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;

namespace parser {
	
	typedef std::vector<std::uint8_t> bytevector;
	
	class Packet {
	public:
		Packet();
		
		//Deserialization constructor
		Packet(const bytevector& data);
		
		virtual ~Packet();
		
		virtual const bytevector serialize() const = 0;
	};
	
}

#endif //ATTACK_PACKET_H
