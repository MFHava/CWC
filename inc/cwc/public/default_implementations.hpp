
//          Copyright Michael Florian Hava.
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

		template<typename Implementation, typename Head, typename... Tail>
		struct default_implementation_chaining<Implementation, ptl::type_list<Head, Tail...>> : vtable_implementation<Head, Implementation, ptl::type_list<Tail...>> {};


		template<typename Implementation, typename AdHocComponent>
		struct default_implementation : default_implementation_chaining<Implementation, typename AdHocComponent::cwc_interfaces::template push_back<AdHocComponent>> {};


		template<typename... Interfaces>
		struct interface_implementation_base : Interfaces... {
			using cwc_interfaces = typename ptl::type_list<component, Interfaces...>::unique;
		};

		template<typename Component, typename... Interfaces>
		struct component_implementation_base : Interfaces..., Component {
			using cwc_interfaces = typename Component::cwc_interfaces::template merge<ptl::type_list<Interfaces...>>::unique;
		};
	}

	//! @brief default implementation of basic operations for use with simple interface-implementations
	//! @tparam Implementation name of the class that implements the interface
	//! @tparam Interface interface to implement
	//! @tparam AdditionalInterfaces additional interfaces to implement
	template<typename Implementation, typename Interface, typename... AdditionalInterfaces>
	struct interface_implementation : 
		internal::default_implementation<
			Implementation,
			internal::interface_implementation_base<
				Interface,
				AdditionalInterfaces...
			>
		> {
	};

	//! @brief default implementation of basic operations for use with component implementations
	//! @tparam Implementation name of the class that implements the component
	//! @tparam Component component to implement
	//! @tparam AdditionalInterfaces additional interfaces to implement
	template<typename Implementation, typename Component, typename... AdditionalInterfaces>
	struct component_implementation :
		internal::default_implementation<
			Implementation,
			internal::component_implementation_base<
				Component,
				AdditionalInterfaces...
			>
		> {

		struct cwc_component_factory : interface_implementation<cwc_component_factory, typename Component::cwc_factory> {
			template<typename... Params>
			auto create(Params &&... params) const -> intrusive_ptr<component> { return make_intrusive<Implementation>(std::forward<Params>(params)...); }
		};
	};
}
