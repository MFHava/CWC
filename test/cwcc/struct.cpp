
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "shared.h"

BOOST_AUTO_TEST_CASE(struct_) {
	const auto & b = parse(
		"bundle UT {\n"
		"	struct Struct0 {\n"
		"		::cwc::int field0;\n"
		"		//test\n"
		"		::cwc::int field1;\n"
		"		::cwc::int field2, field3;\n"
		"		//test\n"
		"		::cwc::int field4, field5;\n"
		"	};\n"
		"	//test\n"
		"	struct Struct1 {\n"
		"		::cwc::int field0;\n"
		"		//test\n"
		"		::cwc::int field1;\n"
		"		::cwc::int field2, field3;\n"
		"		//test\n"
		"		::cwc::int field4, field5;\n"
		"	};\n"
		"}"
	);

	cwcc::bundle reference;
	reference.name = "UT";
	auto append = [&](std::vector<cwcc::documentation> lines, std::string name) {
		cwcc::struct_ s;
		s.lines = std::move(lines);
		s.name = std::move(name);
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.type = cwcc::untemplated_type{"::cwc::int"};
			m.fields.push_back("field0");
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.lines.push_back(cwcc::documentation{"test"});
			m.type = cwcc::untemplated_type{"::cwc::int"};
			m.fields.push_back("field1");
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.type = cwcc::untemplated_type{"::cwc::int"};
			m.fields.push_back("field2");
			m.fields.push_back("field3");
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.lines.push_back(cwcc::documentation{"test"});
			m.type = cwcc::untemplated_type{"::cwc::int"};
			m.fields.push_back("field4");
			m.fields.push_back("field5");
			return m;
		}());
		reference.members.push_back(s);
	};

	append({},         "Struct0");
	append({{"test"}}, "Struct1");

	BOOST_TEST((b == reference));
}