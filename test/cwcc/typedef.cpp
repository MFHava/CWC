
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "shared.h"

BOOST_AUTO_TEST_CASE(typedef_) {
	const auto & b = parse(
		"bundle UT {\n"
		"	using Type0 = ::cwc::int;\n"
		"	using Type1 = mutable ::cwc::int;\n"
		"	using Type2 = ::cwc::int[];\n"
		"	using Type3 = mutable ::cwc::int[];\n"
		"	//test\n"
		"	using Type4 = ::cwc::int;\n"
		"	//test\n"
		"	using Type5 = mutable ::cwc::int;\n"
		"	//test\n"
		"	using Type6 = ::cwc::int[];\n"
		"	//test\n"
		"	using Type7 = mutable ::cwc::int[];\n"
		"}"
	);

	cwcc::bundle reference;
	reference.name = "UT";
	auto append = [&](std::vector<cwcc::documentation> lines, std::string name, cwcc::mutability mutable_, bool array) {
		cwcc::typedef_ t;
		t.lines = std::move(lines);
		t.name = std::move(name);
		t.mutable_ = mutable_;
		t.type = "::cwc::int";
		t.array = array;
		reference.members.push_back(t);
	};

	append({},         "Type0", false, false);
	append({},         "Type1", true,  false);
	append({},         "Type2", false, true);
	append({},         "Type3", true,  true);
	append({{"test"}}, "Type4", false, false);
	append({{"test"}}, "Type5", true,  false);
	append({{"test"}}, "Type6", false, true);
	append({{"test"}}, "Type7", true,  true);

	BOOST_TEST((b == reference));
}