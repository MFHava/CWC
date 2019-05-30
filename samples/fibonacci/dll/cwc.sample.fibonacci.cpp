//This file was generated for CWC by CWCC
//ATTENTION: Do not modify this file as its content is highly dependent on the design of CWC!
static
constexpr
auto cwc_definition{
"//! @brief CWC Sample \"Fibonacci Generator\"\n"
"//! @author Michael Florian Hava\n"
"bundle cwc::sample::fibonacci {\n"
"	interface sequence {\n"
"		auto calculate(::cwc::uint8 no) -> ::cwc::uint64;\n"
"	};\n"
"\n"
"	component generator : sequence {};\n"
"}\n"
};
#include <cwc/internal/bundle.hpp>
