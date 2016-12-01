
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "shared.h"

BOOST_AUTO_TEST_CASE(typedef_) {
	const auto & b = parse(
		"bundle UT {\n"
		"	using Type00 =         ::cwc::int;\n"
		"	using Type01 = mutable ::cwc::int;\n"
		"	using Type02 =         ::cwc::int[];\n"
		"	using Type03 = mutable ::cwc::int[];\n"
		"	using Type04 =         ::cwc::int?;\n"
		"	using Type05 = mutable ::cwc::int?;\n"
		"	//test\n"
		"	using Type10 =         ::cwc::int;\n"
		"	//test\n"
		"	using Type11 = mutable ::cwc::int;\n"
		"	//test\n"
		"	using Type12 =         ::cwc::int[];\n"
		"	//test\n"
		"	using Type13 = mutable ::cwc::int[];\n"
		"	//test\n"
		"	using Type14 =         ::cwc::int?;\n"
		"	//test\n"
		"	using Type15 = mutable ::cwc::int?;\n"
		"}"
	);

	cwcc::bundle reference;
	reference.name = "UT";
	auto append = [&](std::vector<cwcc::documentation> lines, std::string name, cwcc::mutability mutable_, boost::optional<cwcc::typedef_::attributes> attribute) {
		cwcc::typedef_ t;
		t.lines = std::move(lines);
		t.name = std::move(name);
		t.mutable_ = mutable_;
		t.type = "::cwc::int";
		t.attribute = attribute;
		reference.members.push_back(t);
	};

	append({},         "Type00", false, boost::none);
	append({},         "Type01", true,  boost::none);
	append({},         "Type02", false, cwcc::typedef_::attributes::array);
	append({},         "Type03", true,  cwcc::typedef_::attributes::array);
	append({},         "Type04", false, cwcc::typedef_::attributes::optional);
	append({},         "Type05", true,  cwcc::typedef_::attributes::optional);
	append({{"test"}}, "Type10", false, boost::none);
	append({{"test"}}, "Type11", true,  boost::none);
	append({{"test"}}, "Type12", false, cwcc::typedef_::attributes::array);
	append({{"test"}}, "Type13", true,  cwcc::typedef_::attributes::array);
	append({{"test"}}, "Type14", false, cwcc::typedef_::attributes::optional);
	append({{"test"}}, "Type15", true,  cwcc::typedef_::attributes::optional);

	BOOST_TEST((b == reference));
}