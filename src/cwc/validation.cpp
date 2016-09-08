
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cwc/cwc.hpp>
#include <limits>
#include <climits>

//validate that plattform conforms to basic CWC ABI requirements
namespace {
	template<typename FloatingPoint, size_t ExpectedSize>
	struct validate_floating_point final {
		enum {
			value = sizeof(FloatingPoint) == ExpectedSize
			     && std::numeric_limits<FloatingPoint>::is_specialized
			     && std::numeric_limits<FloatingPoint>::is_iec559
			     && std::is_floating_point<FloatingPoint>::value
		};
	};
}

static_assert(CHAR_BIT == 8, "unsupported character type (not 8 bits)");
static_assert(validate_floating_point<cwc::float32, 4>::value, "unsupported single precision floating point");
static_assert(validate_floating_point<cwc::float64, 8>::value, "unsupported double precision floating point");