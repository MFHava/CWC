
//          Copyright Michael Florian Hava 2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "shared.h"

BOOST_AUTO_TEST_CASE(delegate) {
	const auto & b = parse(
		"bundle UT {\n"
		"	delegate Delegate0();\n"
		"	delegate Delegate1(::cwc::int val1);\n"
		"	delegate Delegate2(::cwc::int val1, ::cwc::int val2);\n"
		"	//test\n"
		"	delegate Delegate3();\n"
		"	//test\n"
		"	delegate Delegate4(::cwc::int val1);\n"
		"	//test\n"
		"	delegate Delegate5(::cwc::int val1, ::cwc::int val2);\n"
		"}"
	);

	cwcc::bundle reference;
	reference.name = "UT";
	auto append = [&](std::vector<cwcc::documentation> lines, std::string name, std::vector<cwcc::param> params) {
		cwcc::delegate d;
		d.lines = std::move(lines);
		d.name = std::move(name);
		d.params = std::move(params);
		reference.members.push_back(d);
	};

	std::vector<cwcc::param> params0;
	std::vector<cwcc::param> params1;
	std::vector<cwcc::param> params2;
	{
		cwcc::param p;
		p.name = "val1";
		p.type = cwcc::untemplated_type{"::cwc::int"};
		params1.push_back(p);
		params2.push_back(p);
		p.name = "val2";
		params2.push_back(p);
	}

	append({},         "Delegate0", params0);
	append({},         "Delegate1", params1);
	append({},         "Delegate2", params2);

	append({{"test"}}, "Delegate3", params0);
	append({{"test"}}, "Delegate4", params1);
	append({{"test"}}, "Delegate5", params2);

	BOOST_TEST((b == reference));
}
