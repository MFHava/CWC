
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "shared.h"

BOOST_AUTO_TEST_CASE(enumerator) {
	const auto b = parse(
		"bundle UT {\n"
		"	enumerator Enumerator0 -> ::cwc::int;\n"
		"	//test\n"
		"	enumerator Enumerator1 -> ::cwc::int;\n"
		"}"
	);

	cwcc::bundle reference;
	reference.name = "UT";

	auto append = [&](std::vector<cwcc::documentation> lines, std::string name, cwcc::templated_type type) {
		cwcc::enumerator e;
		e.lines = std::move(lines);
		e.name = std::move(name);
		e.type = std::move(type);
		reference.members.push_back(e);
	};

	append({},         "Enumerator0", cwcc::untemplated_type{"::cwc::int"});
	append({{"test"}}, "Enumerator1", cwcc::untemplated_type{"::cwc::int"});

	BOOST_TEST((b == reference));
}