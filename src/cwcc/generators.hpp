
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <istream>
#include <ostream>

namespace cwcc {
	struct bundle;

	void generate_header(std::ostream & out, const bundle & b);
	void generate_stub(std::ostream & out, const bundle & b);
	void generate_source(std::istream & in, std::ostream & out, const bundle & b);

	auto base_file_name(std::string s) -> std::string;
}