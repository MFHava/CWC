
//          Copyright Michael Florian Hava 2012.
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

	template<typename Implementation, typename Head, typename Tail>
	struct default_implementation_chaining<Implementation, TL::type_list<Head, Tail>> : vtable_implementation<Head, Implementation, Tail> {};

	template<typename Implementation, typename VTable>
	struct default_implementation_chaining<Implementation, TL::type_list<VTable, TL::empty_type_list>> : VTable {};//last base has to be vtable to connect implementation to


	template<typename Implementation, typename AdHocComponent>
	struct default_implementation : default_implementation_chaining<Implementation, TL::append_t<typename AdHocComponent::cwc_interfaces, AdHocComponent>> {};


	template<typename... Interfaces>
	struct interface_implementation_base : Interfaces... {
		using cwc_interfaces = make_base_list_t<Interfaces...>;
	};

	template<typename Component, typename... Interfaces>
	struct component_implementation_base : Interfaces..., Component {
		using cwc_interfaces = TL::unique_t<
			TL::append_t<
				typename Component::cwc_interfaces,
				TL::make_type_list_t<Interfaces...>
			>
		>;
	};
}
