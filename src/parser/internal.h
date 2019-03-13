//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_INTERNAL_H
#define ATTACK_INTERNAL_H

#include "MalformedPacketException.h"
#include <vector>
#include <cstdint>
#include <cstring>
#include <boost/fusion/include/algorithm.hpp>
#include <type_traits>
#include <utility>
#include <climits>

typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;

namespace parser {
	typedef std::vector<std::uint8_t> bytevector;
	
	//Use this to make a type not be byteswapped.
	template<typename T>
	class BigEndian {
	public:
		T value;
	};
	
	static_assert(sizeof(BigEndian<uint8_t>) == sizeof(uint8_t), "BigEndian has size?!");
	
	//Generate a mask with the lowest n bits set to 1.
	template<typename T>
	inline constexpr static T generateMask(unsigned long n) {
		return ((((T) 1) << (n - 1)) - 1) | (((T) 1) << (n - 1));
	}
	
	//Constexpr recursive function to byteswap an integer type.
	//Based on https://stackoverflow.com/a/36937049
	template<class T, std::size_t... N>
	constexpr T byteswap_impl(T i, std::index_sequence<N...>) {
		return ((((i >> N * CHAR_BIT) & (T) (unsigned char) (-1)) << ((sizeof(T) - 1 - N) * CHAR_BIT)) | ...);
	};
	
	constexpr uint128_t byteswap(uint128_t i) {
		const auto a = static_cast<const uint32_t>(i);
		const auto b = static_cast<const uint64_t>(i >> 32);
		const auto c = static_cast<const uint32_t>(i >> 96);
		const auto sa = byteswap_impl<uint32_t>(a, std::make_index_sequence<sizeof(uint32_t)>{});
		const auto sb = byteswap_impl<uint64_t>(b, std::make_index_sequence<sizeof(uint64_t)>{});
		const auto sc = byteswap_impl<uint32_t>(c, std::make_index_sequence<sizeof(uint32_t)>{});
		return ((uint128_t)sa << 96) | ((uint128_t)sb << 32) | sc;
	}
	
	constexpr uint128_t byteswap(int128_t i) {
		const auto a = static_cast<const uint32_t>(i);
		const auto b = static_cast<const uint64_t>(i >> 32);
		const auto c = static_cast<const uint32_t>(i >> 96);
		const auto sa = byteswap_impl<uint32_t>(a, std::make_index_sequence<sizeof(uint32_t)>{});
		const auto sb = byteswap_impl<uint64_t>(b, std::make_index_sequence<sizeof(uint64_t)>{});
		const auto sc = byteswap_impl<uint32_t>(c, std::make_index_sequence<sizeof(uint32_t)>{});
		return ((uint128_t)sa << 96) | ((uint128_t)sb << 32) | sc;
	}
	
	template<class T, class U = typename std::make_unsigned<T>::type>
	constexpr T byteswap(T i) {
		return byteswap_impl<U>(i, std::make_index_sequence<sizeof(T)>{});
	}
	
	//Byteswap: Case sequence (declaration)
	template<typename T>
	typename std::enable_if<boost::fusion::traits::is_sequence<T>{}, void>::type
	inline static recursiveEndianSwap(T &in);
	
	//Byteswap: Case integer (declaration)
	template<typename T>
	typename std::enable_if<std::is_integral<T>{}, void>::type
	inline static recursiveEndianSwap(T &in) {
		in = byteswap(in);
	}
	
	//Because is_integral does not recognise 128 bit types.
	inline static void recursiveEndianSwap(uint128_t &in) {
		in = byteswap(in);
	}
	
	inline static void recursiveEndianSwap(int128_t &in) {
		in = byteswap(in);
	}
	
	//Byteswap: Case enum (declaration)
	template<typename T>
	typename std::enable_if<std::is_enum<T>{}, void>::type
	inline static recursiveEndianSwap(T &in) {
		//Enum is essentially an integral type.
		in = static_cast<T>(byteswap(static_cast<typename std::underlying_type<T>::type>(in)));
	}
	
	//Byteswap: Case BigEndian/no-op
	template<typename T>
	void inline static recursiveEndianSwap(BigEndian<T> &in) {}
	
	struct __byteswap {
		template<typename T>
		void
		operator()(T &x) const {
			recursiveEndianSwap(x);
		};
	};
	
	template<typename T>
	typename std::enable_if<boost::fusion::traits::is_sequence<T>{}, void>::type
	inline static recursiveEndianSwap(T &in) {
		boost::fusion::for_each(in, __byteswap());
	}
	
	template<typename T>
	const parser::bytevector
	deserializeObject(T &target, const parser::bytevector &source, const bool &autoEndian = true) {
		if (source.size() < sizeof(T))
			throw parser::MalformedPacketException("Packet too small.");
		
		memset((std::uint8_t *) (&target), 0, sizeof(T));
		memcpy((std::uint8_t *) (&target), source.data(), sizeof(T));
		
		if (autoEndian) {
			if (__BYTE_ORDER__ != __ORDER_BIG_ENDIAN__) {
				recursiveEndianSwap(target);
			}
		}
		
		return parser::bytevector(source.begin() + sizeof(T), source.end());
	}
	
	template<typename T>
	void serializeObject(parser::bytevector &target, const T &source, const bool &autoEndian = true) {
		T sourceCopy;
		memcpy(&sourceCopy, &source, sizeof(T));
		
		if (autoEndian) {
			if (__BYTE_ORDER__ != __ORDER_BIG_ENDIAN__) {
				recursiveEndianSwap(sourceCopy);
			}
		}
		
		target.insert(target.end(), (const uint8_t *) &sourceCopy, ((const uint8_t *) &sourceCopy) + sizeof(T));
	}
}

#endif //ATTACK_INTERNAL_H
