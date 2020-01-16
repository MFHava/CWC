
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc::internal {
	template<bool IncRefCount, typename TypeList>
	struct cast_to_interface_helper;

	template<bool IncRefCount, typename Head, typename... Tail>
	struct cast_to_interface_helper<IncRefCount, type_list<Head, Tail...>> final {
		template<typename Self>
		static
		void cast(Self * self, const uuid & id, void ** result) {
			if(id == interface_id_v<Head>) {
				using IdentityType = typename Self::cwc_interfaces::template at<1>; //does not work for classes that implement no additional interfaces...
				static_assert(!std::is_same_v<IdentityType, component>);

				using Cast = std::conditional_t<std::is_same_v<Head, component>, IdentityType, Head>;
				auto ptr{static_cast<Cast *>(self)};
				if constexpr(IncRefCount) ptr->cwc$component$new$0();
				*result = ptr;
			} else {
				if constexpr(sizeof...(Tail)) return cast_to_interface_helper<IncRefCount, type_list<Tail...>>::cast(self, id, result);
				else throw std::bad_cast{};
			}
		}
	};

	template<bool IncRefCount, typename Self>
	void cast_to_interface(Self * self, const uuid & id, void ** result) { cast_to_interface_helper<IncRefCount, typename Self::cwc_interfaces>::cast(const_cast<std::remove_const_t<Self> *>(static_cast<const Self *>(self)), id, result); }
}
