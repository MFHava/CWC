
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "shared.h"

BOOST_AUTO_TEST_CASE(component) {
	const auto & b = parse(
		"bundle UT {\n"
		"	component C0 : ::cwc::I {\n"
		"		constructor();\n"
		"		constructor(::cwc::int param);\n"
		"		constructor(mutable ::cwc::int param);\n"
		"	};\n"
		"	//test\n"
		"	component C1 : ::cwc::I {\n"
		"		//test\n"
		"		constructor();\n"
		"		//test\n"
		"		constructor(::cwc::int param);\n"
		"		//test\n"
		"		constructor(mutable ::cwc::int param);\n"
		"	};\n"
		"}"
	);

	cwcc::bundle reference;
	reference.name = "UT";

	auto append = [&](std::vector<cwcc::documentation> lines, std::string name) {
		cwcc::component c;
		c.lines = std::move(lines);
		c.name = std::move(name);
		c.interfaces.push_back("::cwc::I");
		c.members.push_back({c.lines, {}});
		c.members.push_back({c.lines,{{false, cwcc::untemplated_type{"::cwc::int"}, "param"}}});
		c.members.push_back({c.lines,{{true,  cwcc::untemplated_type{"::cwc::int"}, "param"}}});
		reference.members.push_back(c);
	};

	append({},         "C0");
	append({{"test"}}, "C1");

	BOOST_TEST((b == reference));
}