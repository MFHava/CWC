
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "fibonacci.h"
#include <cwc/bundle.hpp>

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

namespace {
	//exporting fibonacci as implementation of generator
	//    (there is no need to inherit the actual implementation itself from component_implementation iff intrusive_from_this is NOT needed)
	struct generator : cwc::component_implementation<generator, cwc::sample::fibonacci::generator>, private ::sample::fibonacci {
		//export member functions to component_implementation
		using ::sample::fibonacci::fibonacci;
		using ::sample::fibonacci::calculate;
	};
}

CWC_EXPORT_COMPONENT(cwc::sample::fibonacci::generator, generator);
