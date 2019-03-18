//
// Created by rewbycraft on 2/25/19.
//

#ifndef ATTACK_INTERNAL_H
#define ATTACK_INTERNAL_H

#include <type_traits>
#include "MalformedPacketException.h"
#include <vector>
#include <cstdint>
#include <cstring>
#include <boost/fusion/include/algorithm.hpp>
#include <utility>
#include <climits>
#include "../int128.h"

namespace parser {
	typedef std::vector<std::uint8_t> bytevector;
	
	//Based on https://stackoverflow.com/a/54446459
	template <typename T, bool = std::is_enum<T>::value>
	struct relaxed_underlying_type {
		using type = typename std::underlying_type<T>::type;
	};
	template <typename T>
	struct relaxed_underlying_type<T, false> {
		using type = T;
	};
	
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
	
	template<class T, class U = typename std::make_unsigned<T>::type>
	constexpr T byteswap(T i) {
		return byteswap_impl<U>(i, std::make_index_sequence<sizeof(T)>{});
	}
	
	constexpr uint128_t byteswap(uint128_t i) {
		return std::make_uint128(byteswap(std::uint128_low(i)), byteswap(std::uint128_high(i)));
	}
	
	constexpr uint128_t byteswap(int128_t i) {
		return byteswap((uint128_t) i);
	}
	
	//Byteswap: Case sequence (declaration)
	template<typename T>
	typename std::enable_if<boost::fusion::traits::is_sequence<T>{}, void>::type
	inline static recursiveEndianSwap(T &in);
	
	//Byteswap: Case integer (declaration)
	template<typename T>
	typename std::enable_if<std::is_integral<T>{} || std::is_enum<T>{}, void>::type
	inline static recursiveEndianSwap(T &in) {
		in = static_cast<T>(byteswap(static_cast<typename relaxed_underlying_type<T>::type>(in)));
	}
	
	//Byteswap: Case BigEndian/no-op
	template<typename T>
	void inline static recursiveEndianSwap(BigEndian<T> &in) {}

	template<typename T>
	typename std::enable_if<boost::fusion::traits::is_sequence<T>{}, void>::type
	inline static recursiveEndianSwap(T &in) {
		boost::fusion::for_each(in, [&](auto& x) {
			recursiveEndianSwap(x);
		});
	}
	
	template<typename T>
	typename std::enable_if<!boost::fusion::traits::is_sequence<T>{}, void>::type
	inline static _deserializeObject(T& target, parser::bytevector &source) {
		if (source.size() < sizeof(T))
			throw parser::MalformedPacketException("Packet too small.");
		
		memset((std::uint8_t *) (&target), 0, sizeof(T));
		memcpy((std::uint8_t *) (&target), source.data(), sizeof(T));
		source.erase(source.begin(), source.begin() + sizeof(T));
	}
	
	template<typename T>
	typename std::enable_if<boost::fusion::traits::is_sequence<T>{}, void>::type
	inline static _deserializeObject(T& target, parser::bytevector &source) {
		boost::fusion::for_each(target, [&](auto& x) {
			_deserializeObject(x, source);
		});
	}
	
	
	template<typename T>
	const parser::bytevector
	deserializeObject(T &target, const parser::bytevector &source, const bool &autoEndian = true) {
		parser::bytevector copy = source;
		
		_deserializeObject(target, copy);
		
		if (autoEndian) {
			if (__BYTE_ORDER__ != __ORDER_BIG_ENDIAN__) {
				recursiveEndianSwap(target);
			}
		}
		
		return copy;
	}
	
	template<typename T>
	typename std::enable_if<!boost::fusion::traits::is_sequence<T>{}, void>::type
	inline static _serializeObject(parser::bytevector &target, const T& source) {
		target.insert(target.end(), (const uint8_t *) &source, ((const uint8_t *) &source) + sizeof(T));
	}
	
	template<typename T>
	typename std::enable_if<boost::fusion::traits::is_sequence<T>{}, void>::type
	inline static _serializeObject(parser::bytevector &target, const T& source) {
		boost::fusion::for_each(source, [&](auto& x) {
			_serializeObject(target, x);
		});
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
		
		_serializeObject(target, sourceCopy);
	}
}

#endif //ATTACK_INTERNAL_H
