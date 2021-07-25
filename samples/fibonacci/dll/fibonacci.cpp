
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

extern "C" CWC_EXPORT void CWC_CALL cwc_sample_fibonacci_sequence(int op, void * args[]) noexcept { //TODO: this will be generated by CWCC
	switch(op) {
		case 0: *reinterpret_cast<void **>(args[0]) = new sample::fibonacci; break;
		case 1: *reinterpret_cast<void **>(args[0]) = new sample::fibonacci{*reinterpret_cast<int *>(args[1])}; break;
		case 2: delete reinterpret_cast<sample::fibonacci *>(args[0]); break;
		case 3: *reinterpret_cast<::cwc::uint64 *>(args[2]) = reinterpret_cast<const sample::fibonacci *>(args[0])->calculate(*reinterpret_cast<const ::cwc::uint8 *>(args[1])); break;
		default: cwc::internal::unreachable();
	}
}
