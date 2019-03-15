//
// Created by rewbycraft on 3/15/19.
//

#include "gtest/gtest.h"
#include "util.h"

TEST(UtilTest, HexTest1) {
	const uint16_t a = 0x1234;
	const std::string b = util::to_hex_string(a);
	EXPECT_EQ("0x1234", b);
}

TEST(UtilTest, BinTest1) {
	const uint8_t a = 0x2;
	const std::string b = util::to_bin_string(a);
	EXPECT_EQ("0b00000010", b);
}
