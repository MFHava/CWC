
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc::internal {
	using factory_map = std::map<uuid, intrusive_ptr<component>>;

	extern factory_map & factories;

	static const struct factories_initializer {
		factories_initializer();
		~factories_initializer() noexcept;
	} factories_init;
}

#define CWC_INTERNAL_CAT(lhs, rhs) CWC_INTERNAL_CAT_(lhs, rhs)
#define CWC_INTERNAL_CAT_(lhs, rhs) lhs ## rhs

//! @brief register implementation for export
//! @param[in] Component public component that is being exported
//! @param[in] Implementation the to be exported implementation of the public component
#define CWC_EXPORT_COMPONENT(Component, Implementation)\
	static\
	const\
	auto CWC_INTERNAL_CAT(registration_dummy_, __LINE__){[] {\
		static_assert(std::is_base_of_v<Component, Implementation>, "Implementation does not fulfill the requirements of exported Component");\
		using Factory = Component::cwc_factory;\
		auto & tmp{cwc::internal::factories[cwc::internal::interface_id_v<Factory>]};\
		if(tmp) throw std::logic_error{"detected duplicated export of component"};\
		tmp = cwc::make_intrusive<typename Implementation::cwc_component_factory>();\
		return 0;\
	}()}
