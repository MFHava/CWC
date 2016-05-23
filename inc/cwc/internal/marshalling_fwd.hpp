
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include <atomic>
#include <cassert>
#include <utility>
#include <stdexcept>

namespace cwc {
	namespace internal {
		template<typename Type, bool IsComponent>
		struct marshalled;
	}
}