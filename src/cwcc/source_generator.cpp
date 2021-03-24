
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <map>
#include <sstream>
#include <iterator>
#include <algorithm>
#include "line.hpp"
#include "nodes.hpp"
#include "utils.hpp"
#include "disclaimer.hpp"
#include "generators.hpp"

namespace cwcc {
	void generate_source(std::istream & is, std::ostream & os) {
		file_disclaimer(os, false);
		os << "static\n"
		      "constexpr\n"
		      "auto cwc_definition{\n";
		std::transform(std::istream_iterator<line>{is}, std::istream_iterator<line>{}, std::ostream_iterator<std::string>{os}, [](const std::string & str) {
			std::stringstream ss;
			ss << '\"';
			for(auto c : str) {
				if(c == '"') ss << '\\';
				ss << c;
			}
			ss << "\\n\"\n";
			return ss.str();
		});//copy content of BDL-file
		os << "};\n"
		      "#include <cwc/internal/bundle.hpp>\n";
	}
}
