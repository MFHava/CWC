
//          Copyright Michael Florian Hava 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	namespace internal {
		template<uint8 Byte0, uint8 Byte1, uint8 Byte2, uint8 Byte3, uint8 Byte4, uint8 Byte5, uint8 Byte6, uint8 Byte7, uint8 Byte8, uint8 Byte9, uint8 ByteA, uint8 ByteB, uint8 ByteC, uint8 ByteD, uint8 ByteE, uint8 ByteF>
		struct uuid_constant {
			static
			constexpr
			auto get() noexcept -> uuid { return {Byte0, Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7, Byte8, Byte9, ByteA, ByteB, ByteC, ByteD, ByteE, ByteF}; }
		};

		template<typename Interface>
		struct interface_id;

		template<typename Interface, typename Implementation, typename TypeList>
		struct vtable_implementation;
	}
}
