//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_INTERNAL_H
#define ATTACK_INTERNAL_H

#include "Packet.h"
#include "MalformedPacketException.h"
#include <cstring>
#include <boost/fusion/include/algorithm.hpp>

namespace parser {
	template<typename T>
	const parser::bytevector deserializeObject(T &target, const parser::bytevector &source) {
		if (source.size() < sizeof(T))
			throw parser::MalformedPacketException("Packet too small.");
		
		memset((std::uint8_t*)(&target), 0, sizeof(T));
		memcpy((std::uint8_t*)(&target), source.data(), sizeof(T));
		
		return parser::bytevector(source.begin() + sizeof(T), source.end());
	}
	
	template<typename T>
	void serializeObject(parser::bytevector &target, const T &source) {
		target.insert(target.end(), (const uint8_t*)&source, ((const uint8_t*)&source)+sizeof(T));
	}
	
	//Generate a mask with the lowest n bits set to 1.
	template<typename T>
	inline constexpr static T generateMask(unsigned long n) {
		return ((((T)1) << n)-1) | (((T)1) << n);
	}
	
	template<typename T>
	inline constexpr static T _byteswap(T in, size_t n = 0) {
		return (n < sizeof(T)/2) ? ((((in >> n*8) & 0xFF) << (sizeof(T)-n-1)*8) + (((in >> (sizeof(T)-n-1)*8) & 0xFF) << n*8) + _byteswap(in-(in & (0xFF << (sizeof(T)-n-1)*8))-(in & (0xFF << n*8)), n+1)) : in;
	}
	
	template<typename T>
	typename std::enable_if<std::is_integral<T>{}, void>::type
	inline static byteswap(T& in) {
		in = _byteswap(in);
	}
	
	template<typename T>
	typename std::enable_if<std::is_class<T>{}, void>::type
	inline static byteswap(T& in) {
		boost::fusion::for_each(in, byteswap);
	}

}

#endif //ATTACK_INTERNAL_H
