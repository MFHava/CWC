
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "shared.h"

BOOST_AUTO_TEST_CASE(typedef_) {
	const auto & b = parse(
		"bundle UT {\n"
		"	using Type0 =                   ::cwc::int ;\n"
		"	using Type1 = array_ref<        ::cwc::int>;\n"
		"	using Type2 = array_ref<mutable ::cwc::int>;\n"
		"	using Type3 =  optional<        ::cwc::int>;\n"
		"	//test\n"
		"	using Type4 =                   ::cwc::int ;\n"
		"	//test\n"
		"	using Type5 = array_ref<        ::cwc::int>;\n"
		"	//test\n"
		"	using Type6 = array_ref<mutable ::cwc::int>;\n"
		"	//test\n"
		"	using Type7 =  optional<        ::cwc::int>;\n"
		"}"
	);

	cwcc::bundle reference;
	reference.name = "UT";
	auto append = [&](std::vector<cwcc::documentation> lines, std::string name, cwcc::templated_type type) {
		cwcc::typedef_ t;
		t.lines = std::move(lines);
		t.name = std::move(name);
		t.type = std::move(type);
		reference.members.push_back(t);
	};

	append({},         "Type0", cwcc::untemplated_type{"::cwc::int"});
	append({},         "Type1", cwcc::array_ref{false, cwcc::untemplated_type{"::cwc::int"}});
	append({},         "Type2", cwcc::array_ref{true, cwcc::untemplated_type{"::cwc::int"}});
	append({},         "Type3", cwcc::optional{cwcc::untemplated_type{"::cwc::int"}});

	append({{"test"}}, "Type4", cwcc::untemplated_type{"::cwc::int"});
	append({{"test"}}, "Type5", cwcc::array_ref{false, cwcc::untemplated_type{"::cwc::int"}});
	append({{"test"}}, "Type6", cwcc::array_ref{true, cwcc::untemplated_type{"::cwc::int"}});
	append({{"test"}}, "Type7", cwcc::optional{cwcc::untemplated_type{"::cwc::int"}});

	BOOST_TEST((b == reference));
}