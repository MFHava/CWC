
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
		struct is_component {
			enum {
				value = std::is_base_of<
					component,
					Type
				>::value
			};
		};

		template<typename Type>
		struct is_interface {
			enum {
				value = std::is_base_of<
					component::cwc_interface,
					Type
				>::value
			};
		};

		template<typename Component>
		struct make_interface {
			using type = typename std::conditional<
				std::is_const<Component>::value,
				const typename Component::cwc_interface,
				typename Component::cwc_interface
			>::type;
		};

		template<typename Interface>
		struct make_component {
			using type = typename std::conditional<
				std::is_const<Interface>::value,
				const typename Interface::cwc_wrapper,
				typename Interface::cwc_wrapper
			>::type;
		};
	}
}