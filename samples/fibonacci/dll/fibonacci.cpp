
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "fibonacci.h"

namespace sample {
	auto fibonacci::compute(unsigned char no) const -> unsigned long long {
		if(no > 93) throw std::out_of_range{"fibonacci number 93 is the last to fit into unsigned long long"};
		auto a = 0ULL, b = 1ULL;
		for(auto i = 0; i < no; ++i) {
			const auto c = a + b;
			a = b;
			b = c;
		}
		return a;
	}
}