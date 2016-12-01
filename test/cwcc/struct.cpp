
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
		"		::cwc::int[1] field6;\n"
		"		//test\n"
		"		::cwc::int[1] field7;\n"
		"		::cwc::int[1] field8, field9;\n"
		"		//test\n"
		"		::cwc::int[1] fieldA, fieldB;\n"
		"		::cwc::int[1, 2] fieldC;\n"
		"		//test\n"
		"		::cwc::int[1, 2] fieldD;\n"
		"	};\n"
		"	//test\n"
		"	struct Struct1 {\n"
		"		::cwc::int field0;\n"
		"		//test\n"
		"		::cwc::int field1;\n"
		"		::cwc::int field2, field3;\n"
		"		//test\n"
		"		::cwc::int field4, field5;\n"
		"		::cwc::int[1] field6;\n"
		"		//test\n"
		"		::cwc::int[1] field7;\n"
		"		::cwc::int[1] field8, field9;\n"
		"		//test\n"
		"		::cwc::int[1] fieldA, fieldB;\n"
		"		::cwc::int[1, 2] fieldC;\n"
		"		//test\n"
		"		::cwc::int[1, 2] fieldD;\n"
		"	};\n"
		"	union Union0 {\n"
		"		::cwc::int field0;\n"
		"		//test\n"
		"		::cwc::int field1;\n"
		"		::cwc::int field2, field3;\n"
		"		//test\n"
		"		::cwc::int field4, field5;\n"
		"		::cwc::int[1] field6;\n"
		"		//test\n"
		"		::cwc::int[1] field7;\n"
		"		::cwc::int[1] field8, field9;\n"
		"		//test\n"
		"		::cwc::int[1] fieldA, fieldB;\n"
		"		::cwc::int[1, 2] fieldC;\n"
		"		//test\n"
		"		::cwc::int[1, 2] fieldD;\n"
		"	};\n"
		"	//test\n"
		"	union Union1 {\n"
		"		::cwc::int field0;\n"
		"		//test\n"
		"		::cwc::int field1;\n"
		"		::cwc::int field2, field3;\n"
		"		//test\n"
		"		::cwc::int field4, field5;\n"
		"		::cwc::int[1] field6;\n"
		"		//test\n"
		"		::cwc::int[1] field7;\n"
		"		::cwc::int[1] field8, field9;\n"
		"		//test\n"
		"		::cwc::int[1] fieldA, fieldB;\n"
		"		::cwc::int[1, 2] fieldC;\n"
		"		//test\n"
		"		::cwc::int[1, 2] fieldD;\n"
		"	};\n"
		"}"
	);

	cwcc::bundle reference;
	reference.name = "UT";
	auto append = [&](std::vector<cwcc::documentation> lines, std::string name, bool union_) {
		cwcc::struct_ s;
		s.lines = std::move(lines);
		s.name = std::move(name);
		s.union_ = union_;
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.type = "::cwc::int";
			m.fields.push_back("field0");
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.lines.push_back(cwcc::documentation{"test"});
			m.type = "::cwc::int";
			m.fields.push_back("field1");
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.type = "::cwc::int";
			m.fields.push_back("field2");
			m.fields.push_back("field3");
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.lines.push_back(cwcc::documentation{"test"});
			m.type = "::cwc::int";
			m.fields.push_back("field4");
			m.fields.push_back("field5");
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.type = "::cwc::int";
			m.sizes.push_back(1);
			m.fields.push_back("field6");
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.lines.push_back(cwcc::documentation{"test"});
			m.type = "::cwc::int";
			m.sizes.push_back(1);
			m.fields.push_back("field7");
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.type = "::cwc::int";
			m.sizes.push_back(1);
			m.fields.push_back("field8");
			m.fields.push_back("field9");
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.lines.push_back(cwcc::documentation{"test"});
			m.type = "::cwc::int";
			m.sizes.push_back(1);
			m.fields.push_back("fieldA");
			m.fields.push_back("fieldB");
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.type = "::cwc::int";
			m.sizes.push_back(1);
			m.sizes.push_back(2);
			m.fields.push_back("fieldC");
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.lines.push_back(cwcc::documentation{"test"});
			m.type = "::cwc::int";
			m.sizes.push_back(1);
			m.sizes.push_back(2);
			m.fields.push_back("fieldD");
			return m;
		}());
		
		reference.members.push_back(s);
	};

	append({},         "Struct0", false);
	append({{"test"}}, "Struct1", false);
	append({},         "Union0",  true);
	append({{"test"}}, "Union1",  true);

	BOOST_TEST((b == reference));
}