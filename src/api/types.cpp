
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <cwc/cwc.hpp>

namespace cwc {
	static_assert(sizeof(uuid) == 16, "unexpected size of uuid detected");

	auto operator==(const uuid & lhs, const uuid & rhs) -> bool { return std::equal(lhs.data, lhs.data + 16, rhs.data); }

	auto operator!=(const uuid & lhs, const uuid & rhs) -> bool { return !(lhs == rhs); }
}