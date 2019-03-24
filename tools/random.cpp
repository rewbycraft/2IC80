//
// Created by rewbycraft on 3/20/19.
//

#include "parser/internal.h"
#include <boost/fusion/include/adapt_struct.hpp>

struct test_t {
	uint32_t a,b;
};

BOOST_FUSION_ADAPT_STRUCT(test_t, (uint32_t, a), (uint32_t, b))

struct test_t test(parser::bytevector& vec) {
	struct test_t t;
	parser::deserializeObject(t, vec);
	return t;
}

int main(int argc, char* argv[]) {

}