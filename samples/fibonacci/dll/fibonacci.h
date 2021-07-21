
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "../cwc.sample.fibonacci.cwch"

namespace sample {
	struct fibonacci {
		fibonacci() noexcept { printf("%s()\n", __func__); }

		~fibonacci() noexcept { printf("%s()\n", __func__); }

		auto calculate(::cwc::uint8 no) const -> ::cwc::uint64;
		void calculate(::cwc::uint8 first, ::cwc::uint8 last, const ::cwc::sample::fibonacci::handler & callback) const;
	};
}
