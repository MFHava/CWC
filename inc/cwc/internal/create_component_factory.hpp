
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include <cstring>

namespace cwc {
	namespace internal {
		template<typename TypeList>
		struct create_component_factory final {
			static void create(const ascii * fqn, component::cwc_interface ** result) {
				using Type = typename TypeList::head;
				if(std::strcmp(fqn, Type::cwc_fqn())) create_component_factory<typename TypeList::tail>::create(fqn, result);
				else *result = new typename Type::cwc_component_factory;
			}
		};

		template<>
		struct create_component_factory<TL::empty_type_list> { //NOTE: final
			static void create(const ascii *, component::cwc_interface **) { throw std::logic_error{"unsupported component"}; }
		};
	}
}