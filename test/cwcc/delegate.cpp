
//          Copyright Michael Florian Hava 2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "shared.h"

BOOST_AUTO_TEST_CASE(delegate) {
	const auto & b = parse(
		"bundle UT {\n"
		"	delegate void VDelegate0();\n"
		"	delegate void VDelegate1(::cwc::int val1);\n"
		"	delegate void VDelegate2(::cwc::int val1, ::cwc::int val2);\n"
		"	//test\n"
		"	delegate void VDelegate3();\n"
		"	//test\n"
		"	delegate void VDelegate4(::cwc::int val1);\n"
		"	//test\n"
		"	delegate void VDelegate5(::cwc::int val1, ::cwc::int val2);\n"
		"	delegate auto ADelegate0() -> ::cwc::int;\n"
		"	delegate auto ADelegate1(::cwc::int val1) -> ::cwc::int;\n"
		"	delegate auto ADelegate2(::cwc::int val1, ::cwc::int val2) -> ::cwc::int;\n"
		"	//test\n"
		"	delegate auto ADelegate3() -> ::cwc::int;\n"
		"	//test\n"
		"	delegate auto ADelegate4(::cwc::int val1) -> ::cwc::int;\n"
		"	//test\n"
		"	delegate auto ADelegate5(::cwc::int val1, ::cwc::int val2) -> ::cwc::int;\n"
		"}"
	);

	cwcc::bundle reference;
	reference.name = "UT";
	auto append = [&](std::vector<cwcc::documentation> lines, std::string name, std::vector<cwcc::param> params, boost::optional<cwcc::untemplated_type> out) {
		cwcc::delegate d;
		d.lines = std::move(lines);
		d.name = std::move(name);
		d.in = std::move(params);
		d.out = out;
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

	append({},         "VDelegate0", params0, boost::none);
	append({},         "VDelegate1", params1, boost::none);
	append({},         "VDelegate2", params2, boost::none);
	append({{"test"}}, "VDelegate3", params0, boost::none);
	append({{"test"}}, "VDelegate4", params1, boost::none);
	append({{"test"}}, "VDelegate5", params2, boost::none);
	append({},         "ADelegate0", params0, cwcc::untemplated_type{"::cwc::int"});
	append({},         "ADelegate1", params1, cwcc::untemplated_type{"::cwc::int"});
	append({},         "ADelegate2", params2, cwcc::untemplated_type{"::cwc::int"});
	append({{"test"}}, "ADelegate3", params0, cwcc::untemplated_type{"::cwc::int"});
	append({{"test"}}, "ADelegate4", params1, cwcc::untemplated_type{"::cwc::int"});
	append({{"test"}}, "ADelegate5", params2, cwcc::untemplated_type{"::cwc::int"});

	BOOST_TEST((b == reference));
}
