
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	//! @defgroup cwc_abi_types Fundamental ABI types
	//! @{
	using int8    = std::int8_t;//!< @brief signed 8bit integer
	using uint8   = std::uint8_t;//!< @brief unsigned  8bit integer
	using int16   = std::int16_t;//!< @brief signed 16bit integer
	using uint16  = std::uint16_t;//!< @brief unsigned 16bit integer
	using int32   = std::int32_t;//!< @brief signed 32bit integer
	using uint32  = std::uint32_t;//!< @brief unsigned 32bit integer
	using int64   = std::int64_t;//!< @brief signed 64bit integer
	using uint64  = std::uint64_t;//!< @brief unsigned 64bit integer
	using float32 = float;//!< @brief single precision (32bit) floating point
	using float64 = double;//!< @brief double precision (64bit) floating point
	using boolean = bool;
	//! @}
}