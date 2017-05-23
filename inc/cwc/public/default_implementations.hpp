
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	//! @brief default implementation of basic operations for use with simple interface-implementations
	//! @tparam Interface interface to implement
	//! @tparam Implementation name of the class that implements the interface
	template<typename Interface, typename Implementation>
	struct interface_implementation :
		internal::default_implementation_chaining<
			Implementation,
			typename internal::TL::append<
				typename internal::make_base_list<Interface>::type,
				Interface
			>::type
		> {

		using cwc_interfaces = typename internal::make_base_list<Interface>::type;
	};

	//! @brief default implementation of basic operations for use with component implementations
	//! @tparam Component component to implement
	//! @tparam Implementation name of the class that implements the component
	template<typename Component, typename Implementation>
	struct component_implementation :
		internal::default_implementation_chaining<
			Implementation,
			typename internal::TL::append<
				typename Component::cwc_interfaces,
				Component
			>::type
		> {

		struct cwc_component_factory : interface_implementation<typename Component::cwc_factory, cwc_component_factory> {
			template<typename... Params>
			auto create(Params &&... params) const -> intrusive_ptr<component> { return intrusive_ptr<Implementation>{new Implementation{std::forward<Params>(params)...}}; }
		};
	};
}