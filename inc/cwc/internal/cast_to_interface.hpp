
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc::internal {
	template<typename Self, typename TypeList, bool IncRefCount>
	class cast_to_interface_helper final {
		using IdentityType = TL::at_t<
			typename Self::cwc_interfaces,
			1
		>;//does not work for classes that implement no additional interfaces...
		static_assert(!std::is_same_v<IdentityType, component>);
	public:
		static
		void cast(Self * self, const uuid & id, void ** result) {
			using Type = typename TypeList::head;
			if(id != interface_id<Type>::get()) return cast_to_interface_helper<Self, typename TypeList::tail, IncRefCount>::cast(self, id, result);
			using Cast = std::conditional_t<std::is_same_v<Type, component>, IdentityType, Type>;
			auto ptr{static_cast<Cast *>(self)};
			if constexpr(IncRefCount) ptr->cwc$component$new$0();
			*result = ptr;
		}
	};

	template<typename Self, bool IncRefCount>
	class cast_to_interface_helper<Self, TL::empty_type_list, IncRefCount> final {
	public:
		static
		void cast(Self *, const uuid &, void **) { throw std::bad_cast(); }
	};

	template<bool IncRefCount = true, typename Self>
	void cast_to_interface(Self * self, const uuid & id, void ** result) {
		auto ptr{const_cast<std::remove_const_t<Self> *>(static_cast<const Self *>(self))};
		cast_to_interface_helper<std::remove_pointer_t<decltype(ptr)>, typename Self::cwc_interfaces, IncRefCount>::cast(ptr, id, result);
	}
}
