
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "shared.h"

BOOST_AUTO_TEST_CASE(delegate) {
	const auto & b = parse(
		"bundle UT {\n"
		"	delegate Delegate0();\n"
		"	//test\n"
		"	delegate Delegate1();\n"
		"	delegate Delegate2() mutable;\n"
		"	//test\n"
		"	delegate Delegate3() mutable;\n"
		"	delegate Delegate4(::cwc::int param);\n"
		"	//test\n"
		"	delegate Delegate5(::cwc::int param);\n"
		"	delegate Delegate6(::cwc::int param) mutable;\n"
		"	//test\n"
		"	delegate Delegate7(::cwc::int param) mutable;\n"
		"	delegate Delegate8(mutable ::cwc::int param);\n"
		"	//test\n"
		"	delegate Delegate9(mutable ::cwc::int param);\n"
		"	delegate DelegateA(mutable ::cwc::int param) mutable;\n"
		"	//test\n"
		"	delegate DelegateB(mutable ::cwc::int param) mutable;\n"
		"}"
	);

	cwcc::bundle reference;
	reference.name = "UT";

	auto append = [&](std::vector<cwcc::documentation> lines, std::string name, std::vector<cwcc::param> in, bool mutable_) {
		cwcc::delegate d;
		d.lines = std::move(lines);
		d.name = std::move(name);
		d.in = std::move(in);
		d.mutable_ = mutable_;
		reference.members.push_back(d);
	};

	append({},         "Delegate0", {}, false);
	append({{"test"}}, "Delegate1", {}, false);
	append({},         "Delegate2", {}, true);
	append({{"test"}}, "Delegate3", {}, true);
	append({},         "Delegate4", {{false, "::cwc::int", "param"}}, false);
	append({{"test"}}, "Delegate5", {{false, "::cwc::int", "param"}}, false);
	append({},         "Delegate6", {{false, "::cwc::int", "param"}}, true);
	append({{"test"}}, "Delegate7", {{false, "::cwc::int", "param"}}, true);
	append({},         "Delegate8", {{true,  "::cwc::int", "param"}}, false);
	append({{"test"}}, "Delegate9", {{true,  "::cwc::int", "param"}}, false);
	append({},         "DelegateA", {{true,  "::cwc::int", "param"}}, true);
	append({{"test"}}, "DelegateB", {{true,  "::cwc::int", "param"}}, true);

	BOOST_TEST((b == reference));
}