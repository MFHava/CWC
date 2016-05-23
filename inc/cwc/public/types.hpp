
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include <cstdint>

namespace cwc {
	using ascii = char;//!< @brief only the lower 7 bits are interpreted (as signedness of char is implementation defined)
	using ascii_string = const ascii *;//!< @brief a basic string as array of ascii, end is marked via '\0'
	using int8 = std::int8_t;//!< @brief signed 8bit integer
	using uint8 = std::uint8_t;//!< @brief unsigned  8bit integer
	using int16 = std::int16_t;//!< @brief signed 16bit integer
	using uint16 = std::uint16_t;//!< @brief unsigned 16bit integer
	using int32 = std::int32_t;//!< @brief signed 32bit integer
	using uint32 = std::uint32_t;//!< @brief unsigned 32bit integer
	using int64 = std::int64_t;//!< @brief signed 64bit integer
	using uint64 = std::uint64_t;//!< @brief unsigned 64bit integer
	using float32 = float;//!< @brief single precision (32bit) floating point
	using float64 = double;//!< @brief double precision (64bit) floating point
	using boolean = uint8;//!< @brief portable boolean (as bool lacks size guarantee)

	//! @brief versioning information of context
	CWC_PACK_BEGIN
	struct version {
		uint16 major;//!< @brief major version
		uint16 minor;//!< @brief minor version
		uint16 patch;//!< @brief patch version
	};
	CWC_PACK_END

	//! @brief universally unique identifier
	CWC_PACK_BEGIN
	struct uuid {
		uint8 data[16];//!< @brief plain storage of 128bit UUID
	};
	CWC_PACK_END

	//! @brief compares two uuids for equality
	//! @param[in] lhs first uuid
	//! @param[in] rhs second uuid
	//! @returns true iff the two uuids are equal
	auto operator==(const uuid & lhs, const uuid & rhs) -> bool;

	//! @brief compares two uuids for inequality
	//! @param[in] lhs first uuid
	//! @param[in] rhs second uuid
	//! @returns true iff the two uuids are not equal
	auto operator!=(const uuid & lhs, const uuid & rhs) -> bool;

	//! @brief marker type to be used when creating plugin-components
	//! @attention plugin_id is non-owning!
	CWC_PACK_BEGIN
	struct plugin_id {
		ascii_string str;
	};
	CWC_PACK_END
}