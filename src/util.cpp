//
// Created by rewbycraft on 3/15/19.
//

#include "util.h"

const std::function<void(const std::string&)> util::prepend_printer(const std::function<void(const std::string&)>& printer) {
	return [&](std::string str){ printer("| " + str); };
}