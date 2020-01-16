
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc::internal {
	template<uint8... Bytes>
	struct uuid_constant {
		static_assert(sizeof...(Bytes) == 16);

		static
		constexpr
		uuid value{Bytes...};
	};

	template<typename Interface>
	struct interface_id;

	template<typename Interface>
	inline
	constexpr
	auto interface_id_v{interface_id<Interface>::value};

	template<typename Interface, typename Implementation, typename TypeList>
	struct vtable_implementation;

	template<typename Interface, typename Implementation>
	struct vtable_implementation<Interface, Implementation, type_list<>> : Interface {};
}
