
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include <map>

namespace cwc::internal {
	struct factory_map final {
		using function_pointer = handle<component>(*)();

		void add(const uuid & id, function_pointer func);
		auto get(const uuid & id) const -> function_pointer;
	private:
		std::map<uuid, function_pointer> mapping;
	};

	extern factory_map & factories;

	static
	const
	struct factories_initializer {
		factories_initializer();
		~factories_initializer() noexcept;
	} factories_init;

	template<typename Implementation, typename... Args>
	auto make_handle(Args &&... args) -> handle<component> {
		using TL = typename Implementation::cwc_interfaces;
		static_assert(!TL::empty);
		static_assert(std::is_same_v<component, typename TL::template at<0>>);
		constexpr auto Index{TL::size == 1 ? 0 : 1};//component is only valid identity if there are no other interfaces
		using Interface = typename TL::template at<Index>;
		//this indirection via an Interface ensures the identity relation for components
		return handle<Interface>{new Implementation{std::forward<Args>(args)...}};
	}

	template<typename Implementation, typename Component>
	struct factory_implementation final : interface_implementation<factory_implementation<Implementation, Component>, typename Component::cwc_factory> {
		template<typename... Params>
		auto create(Params &&... params) const -> handle<component> { return make_handle<Implementation>(std::forward<Params>(params)...); }
	};
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
		static_assert(std::is_base_of_v<Component, Implementation>);\
		using Factory = cwc::internal::factory_implementation<Implementation, Component>;\
		cwc::internal::factories.add(cwc::internal::interface_id<Component::cwc_factory>, &cwc::internal::make_handle<Factory>);\
		return 0;\
	}()}
