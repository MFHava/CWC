
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
}

extern "C" CWC_EXPORT void CWC_CALL cwc_factory(const ::cwc::string_ref * fqn, void * result) {
	assert(fqn);
	assert(result);
	*reinterpret_cast<::cwc::sample::fibonacci::sequence_ *>(result) = ::cwc::sample::fibonacci::sequence_{std::in_place_type<sample::fibonacci>};
}
