
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "../cwc.sample.fibonacci.cwch"

namespace cwc::sample::fibonacci {
	struct sequence::impl {
		impl() noexcept { printf("%s()\n", __func__); }
		impl(int) noexcept { printf("%s(int)\n", __func__); }

		~impl() noexcept { printf("%s()\n", __func__); }

		auto calculate(::cwc::uint8 no) const -> ::cwc::uint64;
	};
}
