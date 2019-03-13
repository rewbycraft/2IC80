//
// Created by rewbycraft on 3/13/19.
//

#ifndef ATTACK_INT128_H
#define ATTACK_INT128_H

#include <cstdint>
#include <type_traits>

//Define the gcc built-in types for 128 bit integers.
typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;

namespace std {
	//Copy integer types to std
	using ::int128_t;
	using ::uint128_t;
	
	//Define the make_unsigned trait for 128 bit integers.
	template<>
	struct make_unsigned<uint128_t> {
		typedef uint128_t type;
	};
	template<>
	struct make_unsigned<int128_t> {
		typedef uint128_t type;
	};
	
	//Define the make_signed trait for 128 bit integers.
	template<>
	struct make_signed<uint128_t> {
		typedef int128_t type;
	};
	template<>
	struct make_signed<int128_t> {
		typedef int128_t type;
	};
	
	//Define the is_integral trait for 128 bit integers.
	template<>
	struct is_integral<uint128_t> : true_type::type { };
	template<>
	struct is_integral<int128_t> : true_type::type { };
	
	//Define a helper function to create 128 bit integers since literals this big aren't supported.
	constexpr static uint128_t make_uint128(uint64_t high, uint64_t low) {
		return (uint128_t)high << 64 | (uint128_t)low;
	}
	constexpr static int128_t make_int128(uint64_t high, uint64_t low) {
		return (uint128_t)std::make_uint128(high, low);
	}
	constexpr static uint64_t uint128_high(uint128_t i) {
		return static_cast<uint64_t>(i >> 64);
	}
	constexpr static uint64_t uint128_low(uint128_t i) {
		return static_cast<uint64_t>(i);
	}
	constexpr static uint64_t int128_high(int128_t i) {
		return static_cast<uint64_t>(i >> 64);
	}
	constexpr static uint64_t int128_low(int128_t i) {
		return static_cast<uint64_t>(i);
	}
}

#endif //ATTACK_INT128_H
