
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <string>
#include <istream>

namespace cwcc {
	struct line : std::string {
		friend auto operator>>(std::istream & in, line & l) -> std::istream & {
			std::getline(in, l);
			return in;
		}
	};
}