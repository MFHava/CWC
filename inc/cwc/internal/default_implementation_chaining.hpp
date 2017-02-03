
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
		template<typename Implementation, typename TypeList>
		struct default_implementation_chaining;

		template<typename Implementation, typename Head, typename Tail>
		struct default_implementation_chaining<Implementation, TL::type_list<Head, Tail>> : Head::template cwc_implementation<Implementation, Tail> {};

		template<typename Implementation, typename VTable>
		struct default_implementation_chaining<Implementation, TL::type_list<VTable, TL::empty_type_list>> : VTable {};//last base has to be vtable to connect implementation to
	}
}