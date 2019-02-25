//
// Created by rewbycraft on 2/25/19.
//

#include "MalformedPacketException.h"

const char *parser::MalformedPacketException::what() const noexcept {
	return reason.c_str();
}

parser::MalformedPacketException::MalformedPacketException(const std::string &reason) : reason(reason) {}
