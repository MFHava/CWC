//This file was generated for CWC by CWCC
//ATTENTION: Do not modify this file as its content is highly dependent on the design of CWC!
static
constexpr
auto cwc_definition{
"//! @brief CWC Sample \"SQLite binding\"\n"
"//! @author Michael Florian Hava\n"
"bundle cwc::sample::sqlite {\n"
"	using blob_ref = array_ref<::cwc::uint8>;\n"
"\n"
"	using entry = variant<\n"
"		::cwc::float32,\n"
"		::cwc::int32,\n"
"		::cwc::int64,\n"
"		::cwc::string_ref,\n"
"		blob_ref\n"
"	>;\n"
"\n"
"	delegate handler(array_ref<entry> result_set);\n"
"\n"
"	interface database {\n"
"		void execute(::cwc::string_ref sql, array_ref<entry> args, handler callback);\n"
"	};\n"
"\n"
"	component file : database {\n"
"		constructor(::cwc::string_ref path);\n"
"	};\n"
"}\n"
};
#include <cwc/internal/bundle.hpp>
