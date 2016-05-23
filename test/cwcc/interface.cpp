
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
		"		auto method()                                 -> mutable ::cwc::int;\n"
		"		auto method() mutable                         -> mutable ::cwc::int;\n"
		"		void method(        ::cwc::int param);\n"
		"		void method(        ::cwc::int param) mutable;\n"
		"		auto method(        ::cwc::int param)         ->         ::cwc::int;\n"
		"		auto method(        ::cwc::int param) mutable ->         ::cwc::int;\n"
		"		auto method(        ::cwc::int param)         -> mutable ::cwc::int;\n"
		"		auto method(        ::cwc::int param) mutable -> mutable ::cwc::int;\n"
		"		void method(mutable ::cwc::int param);\n"
		"		void method(mutable ::cwc::int param) mutable;\n"
		"		auto method(mutable ::cwc::int param)         ->         ::cwc::int;\n"
		"		auto method(mutable ::cwc::int param) mutable ->         ::cwc::int;\n"
		"		auto method(mutable ::cwc::int param)         -> mutable ::cwc::int;\n"
		"		auto method(mutable ::cwc::int param) mutable -> mutable ::cwc::int;\n"
		"		void operator()();\n"
		"		void operator()() mutable;\n"
		"		auto operator()()                                 ->         ::cwc::int;\n"
		"		auto operator()() mutable                         ->         ::cwc::int;\n"
		"		auto operator()()                                 -> mutable ::cwc::int;\n"
		"		auto operator()() mutable                         -> mutable ::cwc::int;\n"
		"		void operator()(        ::cwc::int param);\n"
		"		void operator()(        ::cwc::int param) mutable;\n"
		"		auto operator()(        ::cwc::int param)         ->         ::cwc::int;\n"
		"		auto operator()(        ::cwc::int param) mutable ->         ::cwc::int;\n"
		"		auto operator()(        ::cwc::int param)         -> mutable ::cwc::int;\n"
		"		auto operator()(        ::cwc::int param) mutable -> mutable ::cwc::int;\n"
		"		void operator()(mutable ::cwc::int param);\n"
		"		void operator()(mutable ::cwc::int param) mutable;\n"
		"		auto operator()(mutable ::cwc::int param)         ->         ::cwc::int;\n"
		"		auto operator()(mutable ::cwc::int param) mutable ->         ::cwc::int;\n"
		"		auto operator()(mutable ::cwc::int param)         -> mutable ::cwc::int;\n"
		"		auto operator()(mutable ::cwc::int param) mutable -> mutable ::cwc::int;\n"
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
		"		auto method()                                 -> mutable ::cwc::int;\n"
		"		//test\n"
		"		auto method() mutable                         -> mutable ::cwc::int;\n"
		"		//test\n"
		"		void method(        ::cwc::int param);\n"
		"		//test\n"
		"		void method(        ::cwc::int param) mutable;\n"
		"		//test\n"
		"		auto method(        ::cwc::int param)         ->         ::cwc::int;\n"
		"		//test\n"
		"		auto method(        ::cwc::int param) mutable ->         ::cwc::int;\n"
		"		//test\n"
		"		auto method(        ::cwc::int param)         -> mutable ::cwc::int;\n"
		"		//test\n"
		"		auto method(        ::cwc::int param) mutable -> mutable ::cwc::int;\n"
		"		//test\n"
		"		void method(mutable ::cwc::int param);\n"
		"		//test\n"
		"		void method(mutable ::cwc::int param) mutable;\n"
		"		//test\n"
		"		auto method(mutable ::cwc::int param)         ->         ::cwc::int;\n"
		"		//test\n"
		"		auto method(mutable ::cwc::int param) mutable ->         ::cwc::int;\n"
		"		//test\n"
		"		auto method(mutable ::cwc::int param)         -> mutable ::cwc::int;\n"
		"		//test\n"
		"		auto method(mutable ::cwc::int param) mutable -> mutable ::cwc::int;\n"
		"		//test\n"
		"		void operator()();\n"
		"		//test\n"
		"		void operator()() mutable;\n"
		"		//test\n"
		"		auto operator()()                                 ->         ::cwc::int;\n"
		"		//test\n"
		"		auto operator()() mutable                         ->         ::cwc::int;\n"
		"		//test\n"
		"		auto operator()()                                 -> mutable ::cwc::int;\n"
		"		//test\n"
		"		auto operator()() mutable                         -> mutable ::cwc::int;\n"
		"		//test\n"
		"		void operator()(        ::cwc::int param);\n"
		"		//test\n"
		"		void operator()(        ::cwc::int param) mutable;\n"
		"		//test\n"
		"		auto operator()(        ::cwc::int param)         ->         ::cwc::int;\n"
		"		//test\n"
		"		auto operator()(        ::cwc::int param) mutable ->         ::cwc::int;\n"
		"		//test\n"
		"		auto operator()(        ::cwc::int param)         -> mutable ::cwc::int;\n"
		"		//test\n"
		"		auto operator()(        ::cwc::int param) mutable -> mutable ::cwc::int;\n"
		"		//test\n"
		"		void operator()(mutable ::cwc::int param);\n"
		"		//test\n"
		"		void operator()(mutable ::cwc::int param) mutable;\n"
		"		//test\n"
		"		auto operator()(mutable ::cwc::int param)         ->         ::cwc::int;\n"
		"		//test\n"
		"		auto operator()(mutable ::cwc::int param) mutable ->         ::cwc::int;\n"
		"		//test\n"
		"		auto operator()(mutable ::cwc::int param)         -> mutable ::cwc::int;\n"
		"		//test\n"
		"		auto operator()(mutable ::cwc::int param) mutable -> mutable ::cwc::int;\n"
		"	};\n"
		"}"
	);

	cwcc::bundle reference;
	reference.name = "UT";

	auto append = [&](std::vector<cwcc::documentation> lines, std::string name) {
		cwcc::interface i;
		i.lines = std::move(lines);
		i.name = std::move(name);

		auto append = [&](std::vector<cwcc::documentation> lines, std::string name, std::vector<cwcc::param> in) {
			i.members.push_back({lines, name, in, false});
			i.members.push_back({lines, name, in, true});
			i.members.push_back({lines, name, in, false,{{false, "::cwc::int"}}});
			i.members.push_back({lines, name, in, true,{{false, "::cwc::int"}}});
			i.members.push_back({lines, name, in, false,{{true,  "::cwc::int"}}});
			i.members.push_back({lines, name, in, true,{{true,  "::cwc::int"}}});
		};
		for(auto && name : {"method", "operator()"}) {
			append(i.lines, name, {});
			append(i.lines, name, {{false, "::cwc::int", "param"}});
			append(i.lines, name, {{true,  "::cwc::int", "param"}});
		}

		reference.members.push_back(i);
	};

	append({},         "Interface0");
	append({{"test"}}, "Interface1");

	BOOST_TEST((b == reference));
}