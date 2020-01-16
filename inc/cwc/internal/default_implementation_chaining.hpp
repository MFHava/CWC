
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc::internal {
	template<typename Implementation, typename TypeList>
	struct default_implementation_chaining;

	template<typename Implementation, typename Head, typename... Tail>
	struct default_implementation_chaining<Implementation, type_list<Head, Tail...>> : vtable_implementation<Head, Implementation, type_list<Tail...>> {};

	template<typename Implementation, typename VTable>
	struct default_implementation_chaining<Implementation, type_list<VTable>> : VTable {};//last base has to be vtable to connect implementation to


	template<typename Implementation, typename AdHocComponent>
	struct default_implementation : default_implementation_chaining<Implementation, typename AdHocComponent::cwc_interfaces::template push_back<AdHocComponent>> {};


	template<typename... Interfaces>
	struct interface_implementation_base : Interfaces... {
		using cwc_interfaces = typename type_list<component, Interfaces...>::unique_all;
	};

	template<typename Component, typename... Interfaces>
	struct component_implementation_base : Interfaces..., Component {
		using cwc_interfaces = typename Component::cwc_interfaces::template push_back<type_list<Interfaces...>>::unique;
	};
}
