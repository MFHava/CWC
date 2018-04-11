
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	namespace internal {
		template<typename Type>
		struct interface_id;//TODO: variaable template

		template<typename TypeList>
		struct ambigous_component_helper;

		template<typename Head, typename Tail>
		struct ambigous_component_helper<TL::type_list<Head, Tail>> {
			using type = typename TL::at<
				typename TL::erase_all<
					TL::type_list<Head, Tail>,
					component
				>::type,
				0
			>::type;
		};

		template<>
		struct ambigous_component_helper<TL::type_list<component, TL::empty_type_list>> {
			using type = component;
		};

		template<typename Self, typename TypeList>//TODO: TypeList should be deduced, not passed by the caller
		class cast_to_interface final {
			enum { IsConst = std::is_const<Self>::value };
			using ResultType = typename std::conditional<IsConst, const void, void>::type;
			using IdentityType = typename ambigous_component_helper<TypeList>::type;
		public:
		
			static
			void cast(Self * self, const uuid & id, ResultType ** result) {
				using Type = typename TypeList::head;
				if(id != interface_id<Type>::get()) return cast_to_interface<Self, typename TypeList::tail>::cast(self, id, result);
				using Cast = typename std::conditional<std::is_same<Type, component>::value, IdentityType, Type>::type;
				static_assert(std::is_base_of<Type, Cast>::value, "replacement type is not compatible with target type");
				auto ptr = static_cast<typename std::conditional<IsConst, const Cast, Cast>::type *>(self);
				ptr->cwc$component$new$0();
				*result = ptr;
			}
		};

		template<typename Self>
		class cast_to_interface<Self, TL::empty_type_list> final {
			enum { IsConst = std::is_const<Self>::value };
			using ResultType = typename std::conditional<IsConst, const void, void>::type;
		public:
			static
			void cast(Self *, const uuid &, ResultType **) { throw std::bad_cast(); }
		};
	}
}
