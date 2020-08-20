
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	class component;

	namespace internal {
		using uuid = ptl::array<uint8, 16>;

		template<uint8... Bytes>
		inline
		constexpr
		uuid make_uuid{Bytes...};

		template<typename Interface>
		inline
		constexpr
		void * interface_id{nullptr};

		template<typename Interface>
		inline
		constexpr
		auto interface_id<const Interface>{interface_id<Interface>};

		template<typename Interface, typename Implementation, typename TypeList>
		struct vtable_wrapper;

		template<typename Interface, typename Implementation>
		struct vtable_wrapper<Interface, Implementation, ptl::type_list<>> : Interface {};

		template<typename Implementation, typename TypeList>
		struct default_implementation_chaining : vtable_wrapper<typename TypeList::template at<0>, Implementation, typename TypeList::template erase_at<0>> {};

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
	};
}
