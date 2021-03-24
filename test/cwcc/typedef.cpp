
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "shared.h"

BOOST_AUTO_TEST_CASE(typedef_) {
	const auto & b = parse(
		"bundle UT {\n"
		"	using Type0 = ::cwc::int;\n"
		"	using Type1   = array<::cwc::int, 10>;\n"
		"	using Type2   = array_ref<::cwc::int>;\n"
		"	using Type2m  = array_ref<mutable ::cwc::int>;\n"
		"	using Type3   = bitset<10>;\n"
		"	using Type4   = optional<::cwc::int>;\n"
		"	using Type5   = optional_ref<::cwc::int>;\n"
		"	using Type5m  = optional_ref<mutable ::cwc::int>;\n"
		"	using Type6   = tuple<::cwc::int, ::cwc::float>;\n"
		"	using Type7   = variant<::cwc::int, ::cwc::float>;\n"
		"	using Type8   = variant_ref<::cwc::int, ::cwc::float>;\n"
		"	using Type8m1 = variant_ref<mutable ::cwc::int, ::cwc::float>;\n"
		"	using Type8m2 = variant_ref<::cwc::int, mutable ::cwc::float>;\n"
		"	using Type8m3 = variant_ref<mutable ::cwc::int, mutable ::cwc::float>;\n"
		"	using Type9   = vector<::cwc::int>;\n"
		"	//test\n"
		"	using type0 = ::cwc::int;\n"
		"	//test\n"
		"	using type1   = array<::cwc::int, 10>;\n"
		"	//test\n"
		"	using type2   = array_ref<::cwc::int>;\n"
		"	//test\n"
		"	using type2m  = array_ref<mutable ::cwc::int>;\n"
		"	//test\n"
		"	using type3   = bitset<10>;\n"
		"	//test\n"
		"	using type4   = optional<::cwc::int>;\n"
		"	//test\n"
		"	using type5   = optional_ref<::cwc::int>;\n"
		"	//test\n"
		"	using type5m  = optional_ref<mutable ::cwc::int>;\n"
		"	//test\n"
		"	using type6   = tuple<::cwc::int, ::cwc::float>;\n"
		"	//test\n"
		"	using type7   = variant<::cwc::int, ::cwc::float>;\n"
		"	//test\n"
		"	using type8   = variant_ref<::cwc::int, ::cwc::float>;\n"
		"	//test\n"
		"	using type8m1 = variant_ref<mutable ::cwc::int, ::cwc::float>;\n"
		"	//test\n"
		"	using type8m2 = variant_ref<::cwc::int, mutable ::cwc::float>;\n"
		"	//test\n"
		"	using type8m3 = variant_ref<mutable ::cwc::int, mutable ::cwc::float>;\n"
		"	//test\n"
		"	using type9   = vector<::cwc::int>;\n"
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

	append({},         "Type0",   cwcc::untemplated_type{"::cwc::int"});
	append({},         "Type1",   cwcc::array{cwcc::untemplated_type{"::cwc::int"}, 10});
	append({},         "Type2",   cwcc::array_ref{cwcc::mutability_type{false, cwcc::untemplated_type{"::cwc::int"}}});
	append({},         "Type2m",  cwcc::array_ref{cwcc::mutability_type{true,  cwcc::untemplated_type{"::cwc::int"}}});
	append({},         "Type3",   cwcc::bitset{10});
	append({},         "Type4",   cwcc::optional{cwcc::untemplated_type{"::cwc::int"}});
	append({},         "Type5",   cwcc::optional_ref{cwcc::mutability_type{false, cwcc::untemplated_type{"::cwc::int"}}});
	append({},         "Type5m",  cwcc::optional_ref{cwcc::mutability_type{true,  cwcc::untemplated_type{"::cwc::int"}}});
	append({},         "Type6",   cwcc::tuple{{cwcc::untemplated_type{"::cwc::int"}, cwcc::untemplated_type{"::cwc::float"}}});
	append({},         "Type7",   cwcc::variant{{cwcc::untemplated_type{"::cwc::int"}, cwcc::untemplated_type{"::cwc::float"}}});
	append({},         "Type8",   cwcc::variant_ref{{cwcc::mutability_type{false, cwcc::untemplated_type{"::cwc::int"}}, cwcc::mutability_type{false, cwcc::untemplated_type{"::cwc::float"}}}});
	append({},         "Type8m1", cwcc::variant_ref{{cwcc::mutability_type{true, cwcc::untemplated_type{"::cwc::int"}}, cwcc::mutability_type{false, cwcc::untemplated_type{"::cwc::float"}}}});
	append({},         "Type8m2", cwcc::variant_ref{{cwcc::mutability_type{false, cwcc::untemplated_type{"::cwc::int"}}, cwcc::mutability_type{true, cwcc::untemplated_type{"::cwc::float"}}}});
	append({},         "Type8m3", cwcc::variant_ref{{cwcc::mutability_type{true, cwcc::untemplated_type{"::cwc::int"}}, cwcc::mutability_type{true, cwcc::untemplated_type{"::cwc::float"}}}});
	append({},         "Type9",   cwcc::vector{cwcc::untemplated_type{"::cwc::int"}});
	append({{"test"}}, "type0",   cwcc::untemplated_type{"::cwc::int"});
	append({{"test"}}, "type1",   cwcc::array{cwcc::untemplated_type{"::cwc::int"}, 10});
	append({{"test"}}, "type2",   cwcc::array_ref{cwcc::mutability_type{false, cwcc::untemplated_type{"::cwc::int"}}});
	append({{"test"}}, "type2m",  cwcc::array_ref{cwcc::mutability_type{true,  cwcc::untemplated_type{"::cwc::int"}}});
	append({{"test"}}, "type3",   cwcc::bitset{10});
	append({{"test"}}, "type4",   cwcc::optional{cwcc::untemplated_type{"::cwc::int"}});
	append({{"test"}}, "type5",   cwcc::optional_ref{cwcc::mutability_type{false, cwcc::untemplated_type{"::cwc::int"}}});
	append({{"test"}}, "type5m",  cwcc::optional_ref{cwcc::mutability_type{true,  cwcc::untemplated_type{"::cwc::int"}}});
	append({{"test"}}, "type6",   cwcc::tuple{{cwcc::untemplated_type{"::cwc::int"}, cwcc::untemplated_type{"::cwc::float"}}});
	append({{"test"}}, "type7",   cwcc::variant{{cwcc::untemplated_type{"::cwc::int"}, cwcc::untemplated_type{"::cwc::float"}}});
	append({{"test"}}, "type8",   cwcc::variant_ref{{cwcc::mutability_type{false, cwcc::untemplated_type{"::cwc::int"}}, cwcc::mutability_type{false, cwcc::untemplated_type{"::cwc::float"}}}});
	append({{"test"}}, "type8m1", cwcc::variant_ref{{cwcc::mutability_type{true, cwcc::untemplated_type{"::cwc::int"}}, cwcc::mutability_type{false, cwcc::untemplated_type{"::cwc::float"}}}});
	append({{"test"}}, "type8m2", cwcc::variant_ref{{cwcc::mutability_type{false, cwcc::untemplated_type{"::cwc::int"}}, cwcc::mutability_type{true, cwcc::untemplated_type{"::cwc::float"}}}});
	append({{"test"}}, "type8m3", cwcc::variant_ref{{cwcc::mutability_type{true, cwcc::untemplated_type{"::cwc::int"}}, cwcc::mutability_type{true, cwcc::untemplated_type{"::cwc::float"}}}});
	append({{"test"}}, "type9",   cwcc::vector{cwcc::untemplated_type{"::cwc::int"}});

	BOOST_TEST((b == reference));
}