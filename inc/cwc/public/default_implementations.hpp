
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
	//! @tparam CastTable mapping for custom casts (needed for ambiguous base classes)
	template<typename Interface, typename Implementation, typename CastTable = TL::empty_type_list>
	struct interface_implementation :
		internal::default_implementation_chaining<
			Implementation,
			typename TL::append<
				typename internal::make_base_list<Interface>::type,
				Interface
			>::type
		> {

		using cwc_cast_table = CastTable;
		using cwc_interfaces = typename internal::make_base_list<Interface>::type;
	};

	//! @brief default implementation of basic operations for use with component implementations
	//! @tparam Component component to implement
	//! @tparam Implementation name of the class that implements the component
	//! @tparam CastTable mapping for custom casts (needed for ambiguous base classes)
	template<typename Component, typename Implementation, typename CastTable = TL::empty_type_list>
	struct component_implementation :
		internal::default_implementation_chaining<
			Implementation,
			typename TL::append<
				typename Component::cwc_interfaces,
				Component
			>::type
		> {

		using cwc_cast_table = CastTable;
		struct cwc_component_factory : interface_implementation<typename Component::cwc_factory, cwc_component_factory> {
			template<typename... Params>
			auto create(Params &&... params) const -> intrusive_ptr<component> { return intrusive_ptr<component>(new Implementation{std::forward<Params>(params)...}); }
		};
	};
}