#include <utility>

//
// Created by rewbycraft on 3/23/19.
//

#include "netns.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <exception>
#include <sstream>
#include <cstring>
#include <experimental/filesystem>
#include "../spdlog/include/spdlog/spdlog.h"

class NSException : public std::exception {
	const std::string w;
	static const std::string helper(const std::string &where, const int &why) {
		std::stringstream ss;
		ss << where;
		ss << ": ";
		ss << std::strerror(why);
		ss << " (errno=" << why << ")";
		return ss.str();
	}
public:
	NSException(const std::string &where, const int &why) : w(helper(where,why)) {}
	
	const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT override {
		return w.c_str();
	}
};

inline static const std::experimental::filesystem::path getNSPath(const std::string &str) {
	std::experimental::filesystem::path p = "/var/run/netns";
	p /= str;
	return p;
}

bool netns::exists(const std::string &ns) {
	return std::experimental::filesystem::exists(getNSPath(ns));
}

void netns::enter(const std::string &ns) {
	auto logger = spdlog::get("netns");
	logger->info("Attempting to join network namespace {}.", ns);
	int fd = open(getNSPath(ns).c_str(), O_RDONLY);
	if (fd < 0) {
		throw NSException("open", errno);
	}
	
	if (setns(fd, 0) == -1) {
		throw NSException("setns", errno);
	}
}

