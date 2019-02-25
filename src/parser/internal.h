//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_INTERNAL_H
#define ATTACK_INTERNAL_H

#include "Packet.h"
#include "MalformedPacketException.h"
#include <cstring>

namespace parser {
	template<typename T>
	const parser::bytevector deserializeObject(T &target, const parser::bytevector &source) {
		if (source.size() < sizeof(T))
			throw parser::MalformedPacketException("Packet too small.");
		
		memcpy(&target, source.data(), sizeof(T));
		
		return parser::bytevector(source.begin() + sizeof(T), source.end());
	}
	
	template<typename T>
	void serializeObject(parser::bytevector &target, const T &source) {
		target.insert(target.end(), (const uint8_t*)&source, ((const uint8_t*)&source)+sizeof(T));
	}
}

#endif //ATTACK_INTERNAL_H
