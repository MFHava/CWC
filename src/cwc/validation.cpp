
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <limits>
#include <climits>
#include <cstddef>
#include <type_traits>

namespace {
	template<typename Type, std::size_t Size>
	constexpr //TOOD: in C++20 this should be consteval
	auto validate_floating_point() {
		if constexpr(sizeof(Type) != Size) return false;
		if constexpr(!std::is_floating_point_v<Type>) return false;
		if constexpr(!std::numeric_limits<Type>::is_specialized) return false;
		if constexpr(!std::numeric_limits<Type>::is_iec559) return false;
		return true;
	}

	static_assert(CHAR_BIT == 8);
	static_assert(sizeof(bool) == 1);
	static_assert(validate_floating_point<float,  4>());
	static_assert(validate_floating_point<double, 8>());
	static_assert(sizeof(void *) == sizeof(void(*)()));
}
