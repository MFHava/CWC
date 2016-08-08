
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include <typeinfo>

namespace cwc {
	namespace internal {
		template<typename Interface, typename CastTable, int Index>
		struct interface_cast_selector {
			using type = typename TL::at<
				CastTable,
				Index
			>::type::second_type::cwc_interface;
		};

		template<typename Interface, typename CastTable>
		struct interface_cast_selector<Interface, CastTable, -1> {
			using type = Interface;
		};

		template<typename TypePair, typename Type>
		struct compare_first_type {
			enum {
				value = std::is_same<
					typename TypePair::first_type::cwc_interface,
					Type
				>::value
			};
		};
		
		template<typename Self, typename Interface>
		class interface_cast {
			using CastTable = typename Self::cwc_cast_table;
		public:
			using type = typename interface_cast_selector<
				Interface,
				CastTable,
				TL::find_if<
					CastTable,
					Interface,
					compare_first_type
				>::value
			>::type;
		};

		template<typename Self, typename TypeList>
		struct cast_to_interface final {
			static void cast(Self * self, const uuid & id, void ** result) {
				using Type = typename TypeList::head;
				if(id != Type::cwc_uuid()) return cast_to_interface<Self, typename TypeList::tail>::cast(self, id, result);
				using Cast = typename interface_cast<Self, Type>::type;
				static_assert(std::is_base_of<Type, Cast>::value, "replacement type is not compatible with target type");
				auto ptr = static_cast<Cast *>(self);
				validate(ptr->cwc$component$new$0());
				*result = ptr;
			}
		};

		template<typename Self>
		struct cast_to_interface<Self, TL::empty_type_list> final {
			static void cast(Self *, const uuid &, void **) { throw std::bad_cast(); }
		};
	}
}