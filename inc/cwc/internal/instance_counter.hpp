
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

			static auto get_counter() noexcept -> std::atomic<std::size_t> & {
				static std::atomic<std::size_t> counter{0};
				return counter;
			}
		public:
			instance_counter() { ++get_counter(); }
			~instance_counter() { --get_counter(); }
			static auto get() noexcept -> std::size_t { return get_counter(); }
		};
	}
}