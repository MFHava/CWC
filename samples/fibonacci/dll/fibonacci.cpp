
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

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

	void fibonacci::calculate(const ::cwc::array_ref<const ::cwc::uint8> & nos, ::cwc::array_ref<::cwc::uint64> results) const {
		if(nos.size() > results.size()) throw std::invalid_argument{"results is not big enough"};
		for(std::size_t i{0}; i < nos.size(); ++i)
			results[i] = calculate(nos[i]);
	}
}