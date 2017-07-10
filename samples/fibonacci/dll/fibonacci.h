
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "../cwc.sample.fibonacci.cwch"

namespace sample {
	struct fibonacci : cwc::component_implementation<cwc::sample::fibonacci::generator, fibonacci> {
		fibonacci() =default;
		fibonacci(const ::cwc::int32 & dummy) {}

		auto calculate(::cwc::uint8 no) const -> ::cwc::uint64;
		void calculate(const ::cwc::array_ref<const ::cwc::uint8> & nos, ::cwc::array_ref<::cwc::uint64> results) const;
	};
}