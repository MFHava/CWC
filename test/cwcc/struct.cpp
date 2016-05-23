
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
		"		::cwc::int field6[1];\n"
		"		//test\n"
		"		::cwc::int field7[1];\n"
		"		::cwc::int field8[1], field9[2];\n"
		"		//test\n"
		"		::cwc::int fieldA[1], fieldB[2];\n"
		"		::cwc::int fieldC[1, 2];\n"
		"		//test\n"
		"		::cwc::int fieldD[1, 2];\n"
		"	};\n"
		"	//test\n"
		"	struct Struct1 {\n"
		"		::cwc::int field0;\n"
		"		//test\n"
		"		::cwc::int field1;\n"
		"		::cwc::int field2, field3;\n"
		"		//test\n"
		"		::cwc::int field4, field5;\n"
		"		::cwc::int field6[1];\n"
		"		//test\n"
		"		::cwc::int field7[1];\n"
		"		::cwc::int field8[1], field9[2];\n"
		"		//test\n"
		"		::cwc::int fieldA[1], fieldB[2];\n"
		"		::cwc::int fieldC[1, 2];\n"
		"		//test\n"
		"		::cwc::int fieldD[1, 2];\n"
		"	};\n"
		"	union Union0 {\n"
		"		::cwc::int field0;\n"
		"		//test\n"
		"		::cwc::int field1;\n"
		"		::cwc::int field2, field3;\n"
		"		//test\n"
		"		::cwc::int field4, field5;\n"
		"		::cwc::int field6[1];\n"
		"		//test\n"
		"		::cwc::int field7[1];\n"
		"		::cwc::int field8[1], field9[2];\n"
		"		//test\n"
		"		::cwc::int fieldA[1], fieldB[2];\n"
		"		::cwc::int fieldC[1, 2];\n"
		"		//test\n"
		"		::cwc::int fieldD[1, 2];\n"
		"	};\n"
		"	//test\n"
		"	union Union1 {\n"
		"		::cwc::int field0;\n"
		"		//test\n"
		"		::cwc::int field1;\n"
		"		::cwc::int field2, field3;\n"
		"		//test\n"
		"		::cwc::int field4, field5;\n"
		"		::cwc::int field6[1];\n"
		"		//test\n"
		"		::cwc::int field7[1];\n"
		"		::cwc::int field8[1], field9[2];\n"
		"		//test\n"
		"		::cwc::int fieldA[1], fieldB[2];\n"
		"		::cwc::int fieldC[1, 2];\n"
		"		//test\n"
		"		::cwc::int fieldD[1, 2];\n"
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
			m.fields.push_back({"field0"});
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.lines.push_back(cwcc::documentation{"test"});
			m.type = "::cwc::int";
			m.fields.push_back({"field1"});
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.type = "::cwc::int";
			m.fields.push_back({"field2"});
			m.fields.push_back({"field3"});
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.lines.push_back(cwcc::documentation{"test"});
			m.type = "::cwc::int";
			m.fields.push_back({"field4"});
			m.fields.push_back({"field5"});
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.type = "::cwc::int";
			m.fields.push_back({"field6",{1}});
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.lines.push_back(cwcc::documentation{"test"});
			m.type = "::cwc::int";
			m.fields.push_back({"field7",{1}});
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.type = "::cwc::int";
			m.fields.push_back({"field8",{1}});
			m.fields.push_back({"field9",{2}});
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.lines.push_back(cwcc::documentation{"test"});
			m.type = "::cwc::int";
			m.fields.push_back({"fieldA",{1}});
			m.fields.push_back({"fieldB",{2}});
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.type = "::cwc::int";
			m.fields.push_back({"fieldC",{1, 2}});
			return m;
		}());
		s.members.push_back([] {
			cwcc::struct_::member m;
			m.lines.push_back(cwcc::documentation{"test"});
			m.type = "::cwc::int";
			m.fields.push_back({"fieldD",{1, 2}});
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