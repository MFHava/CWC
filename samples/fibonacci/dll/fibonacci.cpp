
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "fibonacci.h"

namespace cwc::sample::fibonacci {
	auto sequence::impl::calculate(std::uint8_t no) const -> std::uint64_t {
		if(no > 93) throw std::out_of_range{"fibonacci number 93 is the last to fit into uint64"};
		std::uint64_t a{0}, b{1};
		for(decltype(no) i{0}; i < no; ++i) {
			const auto c{a + b};
			a = b;
			b = c;
		}
		return a;
	}

	void sequence::impl::calculate(std::uint8_t first, std::uint8_t last, ptl::function_ref<void(std::uint64_t) noexcept> callback) const {
		if(last < first) throw std::invalid_argument{"last < first"};

		for(; first != last; ++first) callback(calculate(first));
	}
}

CWC_EXPORT_cwc_sample_fibonacci_sequence
