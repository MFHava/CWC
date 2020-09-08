
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	template<typename T>
	struct handle;


	class component;

	namespace internal {
		template<typename Interface>
		inline
		constexpr
		auto interface_id<const Interface>{interface_id<Interface>};


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


		template<typename Implementation, typename Interface, typename... AdditionalInterfaces>
		struct interface_implementation : default_implementation<interface_implementation<Implementation, Interface, AdditionalInterfaces...>, interface_implementation_base<Interface, AdditionalInterfaces...>> {

			template<typename... Args>
			interface_implementation(const handle<component> & loader, Args &&... args) : loader{loader}, self{std::forward<Args>(args)...} {}

			auto cwc_get() const noexcept -> const Implementation & { return self; }
			auto cwc_get()       noexcept ->       Implementation & { return self; }
		private:
			const handle<component> loader;
			Implementation self;
		};


		template<typename Component, typename... Interfaces>
		struct component_implementation_base : Interfaces..., Component {
			using cwc_interfaces = typename Component::cwc_interfaces::template merge<ptl::type_list<Interfaces...>>::unique;
		};


		template<typename Implementation, typename Component, typename... AdditionalInterfaces>
		struct component_implementation : default_implementation<component_implementation<Implementation, Component, AdditionalInterfaces...>, component_implementation_base<Component, AdditionalInterfaces...>> {

			template<typename... Args>
			component_implementation(const handle<component> & loader, Args &&... args) : loader{loader}, self{std::forward<Args>(args)...} {}

			auto cwc_get() const noexcept -> const Implementation & { return self; }
			auto cwc_get()       noexcept ->       Implementation & { return self; }
		private:
			const handle<component> loader;
			Implementation self;
		};
	}
}
