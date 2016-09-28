
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "fibonacci.h"

namespace sample {
	auto fibonacci::operator()(unsigned char no) const -> unsigned long long {
		if(no > 93) throw std::out_of_range{"fibonacci number 93 is the last to fit into unsigned long long"};
		auto a = 0ULL, b = 1ULL;
		for(auto i = 0; i < no; ++i) {
			const auto c = a + b;
			a = b;
			b = c;
		}
		return a;
	}

	void fibonacci::operator()(cwc::array_ref<const unsigned char> nos, cwc::array_ref<unsigned long long> results) const {
		if(nos.size() != results.size()) throw std::invalid_argument{"input and output are not of same length!"};
		for(std::size_t i{0}; i < nos.size(); ++i) results[i] = (*this)(nos[i]);
	}
}