
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "shared.h"

BOOST_AUTO_TEST_CASE(interface) {
	const auto & b = parse(
		"bundle UT {\n"
		"	interface Interface0 {\n"
		"		void method();\n"
		"		void method() mutable;\n"
		"		auto method()                                 ->         ::cwc::int;\n"
		"		auto method() mutable                         ->         ::cwc::int;\n"
		"		void method(        ::cwc::int param);\n"
		"		void method(        ::cwc::int param) mutable;\n"
		"		auto method(        ::cwc::int param)         ->         ::cwc::int;\n"
		"		auto method(        ::cwc::int param) mutable ->         ::cwc::int;\n"
		"		void method(mutable ::cwc::int param);\n"
		"		void method(mutable ::cwc::int param) mutable;\n"
		"		auto method(mutable ::cwc::int param)         ->         ::cwc::int;\n"
		"		auto method(mutable ::cwc::int param) mutable ->         ::cwc::int;\n"
		"	};\n"
		"	//test\n"
		"	interface Interface1 {\n"
		"		//test\n"
		"		void method();\n"
		"		//test\n"
		"		void method() mutable;\n"
		"		//test\n"
		"		auto method()                                 ->         ::cwc::int;\n"
		"		//test\n"
		"		auto method() mutable                         ->         ::cwc::int;\n"
		"		//test\n"
		"		void method(        ::cwc::int param);\n"
		"		//test\n"
		"		void method(        ::cwc::int param) mutable;\n"
		"		//test\n"
		"		auto method(        ::cwc::int param)         ->         ::cwc::int;\n"
		"		//test\n"
		"		auto method(        ::cwc::int param) mutable ->         ::cwc::int;\n"
		"		//test\n"
		"		void method(mutable ::cwc::int param);\n"
		"		//test\n"
		"		void method(mutable ::cwc::int param) mutable;\n"
		"		//test\n"
		"		auto method(mutable ::cwc::int param)         ->         ::cwc::int;\n"
		"		//test\n"
		"		auto method(mutable ::cwc::int param) mutable ->         ::cwc::int;\n"
		"	};\n"
		"}"
	);

	cwcc::bundle reference;
	reference.name = "UT";

	auto append = [&](std::vector<cwcc::documentation> lines, std::string name) {
		cwcc::interface i;
		i.lines = std::move(lines);
		i.name = std::move(name);
		const cwcc::templated_type type{cwcc::untemplated_type{"::cwc::int"}};
		auto append = [&](std::vector<cwcc::documentation> lines, std::vector<cwcc::param> in) {
			i.methods.push_back(cwcc::interface::method{lines, "method", in, false      });
			i.methods.push_back(cwcc::interface::method{lines, "method", in, true       });
			i.methods.push_back(cwcc::interface::method{lines, "method", in, false, type});
			i.methods.push_back(cwcc::interface::method{lines, "method", in, true,  type});
		};
		append(i.lines, {});
		append(i.lines, {{false, type, "param"}});
		append(i.lines, {{true,  type, "param"}});
		reference.members.push_back(i);
	};

	append({},         "Interface0");
	append({{"test"}}, "Interface1");

	BOOST_TEST((b == reference));
}