//
// Created by rewbycraft on 3/15/19.
//

#ifndef ATTACK_UTIL_H
#define ATTACK_UTIL_H

#include <string>
#include <functional>

namespace util {
	template <typename T>
	inline static const std::string to_hex_string(T num) {
		const char* chars = "0123456789abcdef";
		const size_t len = sizeof(T) * 2+2;
		std::string output(len, 'z');
		
		for (size_t i = len - 1; i >= 2; i--) {
			output[i] = chars[num & 0xF];
			num >>= 4;
		}
		
		output[0] = '0';
		output[1] = 'x';
		
		return output;
	}
	
	template <typename T>
	inline static const std::string to_bin_string(T num) {
		const char* chars = "01";
		const size_t len = sizeof(T) * 8+2;
		std::string output(len, 'z');
		
		for (size_t i = len - 1; i >= 2; i--) {
			output[i] = chars[num & 0x1];
			num >>= 1;
		}
		
		output[0] = '0';
		output[1] = 'b';
		
		return output;
	}
	
	const std::function<void(const std::string&)> prepend_printer(const std::function<void(const std::string&)>&);
}

#endif //ATTACK_UTIL_H
