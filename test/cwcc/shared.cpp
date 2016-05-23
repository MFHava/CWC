
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../../src/cwcc/nodes.cpp"
#include "../../src/cwcc/parser.cpp"

auto parse(const char * testcase) -> cwcc::bundle {
	std::stringstream ss{testcase};
	return cwcc::parse(ss);
}