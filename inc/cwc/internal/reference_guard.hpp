
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	namespace internal {
		struct reference_guard {
			reference_guard(component::cwc_interface * ptr) : ptr{ptr} { if(!ptr) throw std::runtime_error{"factory is nullptr"}; }
			~reference_guard() {
				const auto result = ptr->cwc$component$delete$1();
				assert(result == error_code::no_error);
			}
		private:
			component::cwc_interface * ptr;
		};
	}
}