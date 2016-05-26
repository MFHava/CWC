
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
		class instance_counter {
			instance_counter(const instance_counter &) =delete;
			instance_counter(instance_counter &&) =delete;

			auto operator=(const instance_counter &) -> instance_counter & =delete;
			auto operator=(instance_counter &&) -> instance_counter & =delete;
		public:
#ifdef CWC_HOST//no instance counting for host
			instance_counter() =default;
			~instance_counter() =default;
#else
			instance_counter();
			~instance_counter();
#endif
		};
	}
}