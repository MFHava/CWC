
//          Copyright Michael Florian Hava 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <map>
#include "cwc.hpp"

namespace cwc::internal {
	auto registered_factories() noexcept -> std::map<string_ref, intrusive_ptr<component>> &;
}

#define CWC_INTERNAL_CAT(lhs, rhs) CWC_INTERNAL_CAT_(lhs, rhs)
#define CWC_INTERNAL_CAT_(lhs, rhs) lhs ## rhs

//! @brief register implementation for export
//! @param[in] Component public component that is being exporter
//! @param[in] Implementation the to be exported implementation of the public component
#define CWC_EXPORT_COMPONENT(Component, Implementation)\
	static\
	const\
	auto CWC_INTERNAL_CAT(registration_dummy_, __LINE__){([] {\
		static_assert(std::is_base_of_v<Component, Implementation>, "Implementation does not fulfill the requirements of exported Component");\
		auto & tmp{cwc::internal::registered_factories()[Component::cwc_fqn()]};\
		if(tmp) throw std::logic_error{"detected duplicated export of component"};\
		tmp = cwc::make_intrusive<typename Implementation::cwc_component_factory>();\
	}(), 0)}
