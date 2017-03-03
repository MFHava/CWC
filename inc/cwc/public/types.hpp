
//          Copyright Michael Florian Hava 2012.
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
	using utf8    = char;//!< @brief UTF-8 character
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
	using boolean = std::conditional<sizeof(bool) == 1, bool, uint8>::type;//!< @brief portable boolean (as bool lacks size guarantee)
	//! @}

	//! @brief helper to determine if UTF-8 character is a valid ASCII character
	//! @param[in] c character to test
	//! @returns true iff c is a valid ASCII character
	inline
	auto is_ascii(const utf8 & c) -> bool { return (c >= 0 && c < 128); }

	//! @brief universally unique identifier
	CWC_PACK_BEGIN
	struct uuid final {
		uint8 data[16];//!< @brief plain storage of 128bit UUID
	};
	CWC_PACK_END
	static_assert(sizeof(uuid) == 16, "unexpected size of uuid detected");

	//! @brief compares two uuids for equality
	//! @param[in] lhs first uuid
	//! @param[in] rhs second uuid
	//! @returns true iff the two uuids are equal
	inline
	auto operator==(const uuid & lhs, const uuid & rhs) -> bool {
		for(auto i = 0; i < sizeof(uuid::data); ++i)
			if(lhs.data[i] != rhs.data[i])
				return false;
		return true;
	}

	//! @brief compares two uuids for inequality
	//! @param[in] lhs first uuid
	//! @param[in] rhs second uuid
	//! @returns true iff the two uuids are not equal
	inline
	auto operator!=(const uuid & lhs, const uuid & rhs) -> bool { return !(lhs == rhs); }
}