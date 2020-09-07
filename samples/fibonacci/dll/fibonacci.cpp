
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cwc/cwc.hpp>
#include "fibonacci.h"

namespace sample {
	auto fibonacci::calculate(::cwc::uint8 no) const -> ::cwc::uint64 {
		if(no > 93) throw std::out_of_range{"fibonacci number 93 is the last to fit into uint64"};
		::cwc::uint64 a{0}, b{1};
		for(decltype(no) i{0}; i < no; ++i) {
			const auto c{a + b};
			a = b;
			b = c;
		}
		return a;
	}

	void fibonacci::calculate(::cwc::uint8 first, ::cwc::uint8 last, const ::cwc::sample::fibonacci::handler & callback) const {
		if(first > last) throw std::invalid_argument{"fist must be <= last"};
		if(first > 93 || last > 93) throw std::out_of_range{"fibonacci number 93 is the last to fit into uint64"};

		::cwc::uint64 a{0}, b{1};
		for(decltype(first) i{0}; i < last; ++i) {
			if(i >= first) callback(a);
			const auto c{a + b};
			a = b;
			b = c;
		}

	}
}

//export sample::fibonacci as implementation of cwc::sample::fibonacci::generator
CWC_EXPORT_COMPONENT(::sample::fibonacci, cwc::sample::fibonacci::generator);
