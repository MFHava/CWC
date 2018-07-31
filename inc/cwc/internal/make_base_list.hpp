
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc::internal {
	template<typename... Types>
	struct make_base_list {
		using type = TL::unique_t<
			TL::make_type_list_t<
				component,
				Types...
			>
		>;
	};

	template<typename... Types>
	using make_base_list_t = typename make_base_list<Types...>::type;
}
