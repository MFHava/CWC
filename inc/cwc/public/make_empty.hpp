
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	//! @brief create an empty wrapper
	//! @tparam Wrapper interface wrapper to create empty instance for
	//! @returns empty wrapper
	//! @note constness can be enforced by requesting a const Wrapper
	//! @note this function is only supported for interface wrappers, wrappers for components cannot be created with it
	template<typename Wrapper>
	auto make_empty() -> Wrapper {
		return static_cast<typename internal::make_interface<Wrapper>::type *>(nullptr);
	}
}