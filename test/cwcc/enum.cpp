
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "shared.h"

BOOST_AUTO_TEST_CASE(enum_) {
	const auto & b = parse(
		"bundle UT {\n"
		"	enum Enum0 {\n"
		"		Value0,\n"
		"		Value1\n"
		"	};\n"
		"	//test\n"
		"	enum Enum1 {\n"
		"		//test\n"
		"		Value0,\n"
		"		//test\n"
		"		Value1\n"
		"	};\n"
		"}"
	);

	cwcc::bundle reference;
	reference.name = "UT";
	auto append = [&](std::vector<cwcc::documentation> lines, std::string name) {
		cwcc::enum_ e;
		e.lines = std::move(lines);
		e.name = std::move(name);
		e.members.push_back({e.lines, "Value0"});
		e.members.push_back({e.lines, "Value1"});
		reference.members.push_back(e);
	};

	append({},         "Enum0");
	append({{"test"}}, "Enum1");

	BOOST_TEST((b == reference));
}