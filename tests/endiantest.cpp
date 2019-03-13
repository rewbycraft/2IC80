//
// Created by rewbycraft on 3/11/19.
//

#include "gtest/gtest.h"
#include "parser/internal.h"

TEST(EndianTest, ByteSwapTest1) {
	uint32_t a = 0x41424344;
	uint32_t b = parser::byteswap(a);
	EXPECT_EQ(0x44434241, b);
}

TEST(EndianTest, ByteSwapTest2) {
	uint8_t a = 0x41;
	uint8_t b = parser::byteswap(a);
	EXPECT_EQ(0x41, b);
}

TEST(EndianTest, ByteSwapTest3) {
	uint8_t a = 0x1;
	uint8_t b = parser::byteswap(a);
	EXPECT_EQ(0x1, b);
}

TEST(EndianTest, ByteSwapTest4) {
	uint128_t a = std::make_uint128(0x0809101112131415, 0x0001020304050607);
	uint128_t b = parser::byteswap(a);
	uint128_t c = std::make_uint128(0x0706050403020100, 0x1514131211100908);
	EXPECT_EQ(c, b);
}

TEST(EndianTest, AutoEndianTest1) {
	uint8_t a = 0x1;
	parser::recursiveEndianSwap(a);
	EXPECT_EQ(0x1, a);
}
