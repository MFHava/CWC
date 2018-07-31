
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc::internal {
	template<typename TypeList>
	struct ambigous_component_helper;

	template<typename TypeList>
	using ambigous_component_helper_t = typename ambigous_component_helper<TypeList>::type;

	template<typename Head, typename Tail>
	struct ambigous_component_helper<TL::type_list<Head, Tail>> {
		using type = TL::at_t<
			TL::erase_all_t<
				TL::type_list<Head, Tail>,
				component
			>,
			0
		>;
	};

	template<>
	struct ambigous_component_helper<TL::type_list<component, TL::empty_type_list>> {
		using type = component;
	};


	template<typename Self, typename TypeList>//TODO: TypeList should be deduced, not passed by the caller
	class cast_to_interface final {
		enum { IsConst = std::is_const_v<Self> };
		using ResultType = std::conditional_t<IsConst, const void, void>;
		using IdentityType = ambigous_component_helper_t<TypeList>;
	public:
	
		static
		void cast(Self * self, const uuid & id, ResultType ** result) {
			using Type = typename TypeList::head;
			if(id != interface_id<Type>::get()) return cast_to_interface<Self, typename TypeList::tail>::cast(self, id, result);
			using Cast = std::conditional_t<std::is_same_v<Type, component>, IdentityType, Type>;
			static_assert(std::is_base_of_v<Type, Cast>, "replacement type is not compatible with target type");
			auto ptr = static_cast<std::conditional_t<IsConst, const Cast, Cast> *>(self);
			ptr->cwc$component$new$0();
			*result = ptr;
		}
	};

	template<typename Self>
	class cast_to_interface<Self, TL::empty_type_list> final {
	public:
		template<typename ResultType>
		static
		void cast(Self *, const uuid &, ResultType **) { throw std::bad_cast(); }
	};
}
