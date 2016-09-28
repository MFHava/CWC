
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE CWC Tests
#define WIN32_LEAN_AND_MEAN//suppress #define interface
#include <boost/test/unit_test.hpp>
#include <future>
#include "cwc/cwc.hpp"

struct ContextInit {
	ContextInit() {
		cwc::this_context::init(cwc::this_context::init_mode::string, "");
	}

};

BOOST_GLOBAL_FIXTURE(ContextInit);