
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "shared.h"

BOOST_AUTO_TEST_CASE(export_) {
	const auto & b = parse(
		"bundle UT {\n"
		"	interface I1 {};\n"
		"	interface I2 {};\n"
		"	component C1 : I1 {};\n"
		"	component C2 : I2 {};\n"
		"	component C3 : I1, I2 {};\n"
		"	component C4 : I2, I1 {};\n"
		"	export C1;\n"
		"	export C2;\n"
		"	export C3;\n"
		"	export C4;\n"
		"}"
	);

	cwcc::bundle reference;
	reference.name = "UT";
	reference.members.push_back(cwcc::interface{{}, "I1"});
	reference.members.push_back(cwcc::interface{{}, "I2"});
	reference.members.push_back(cwcc::component{{}, "C1", {"I1"}});
	reference.members.push_back(cwcc::component{{}, "C2", {"I2"}});
	reference.members.push_back(cwcc::component{{}, "C3", {"I1", "I2"}});
	reference.members.push_back(cwcc::component{{}, "C4", {"I2", "I1"}});
	reference.members.push_back(cwcc::export_{"C1"});
	reference.members.push_back(cwcc::export_{"C2"});
	reference.members.push_back(cwcc::export_{"C3"});
	reference.members.push_back(cwcc::export_{"C4"});

	BOOST_TEST((b == reference));
}