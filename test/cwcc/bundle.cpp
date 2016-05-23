
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "shared.h"

BOOST_AUTO_TEST_CASE(bundle) {
	const auto b = parse(
		"//test\n"
		"bundle UT {}"
	);
	
	cwcc::bundle reference;
	reference.name = "UT";
	reference.lines.push_back(cwcc::documentation{"test"});

	BOOST_TEST((b == reference));
}