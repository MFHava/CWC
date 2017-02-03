
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	//! @brief entry in the configuration
	CWC_PACK_BEGIN
	struct config_entry final {
		string_view key;//!< @brief key in configuration
		string_view value;//!< @brief associated value
	};
	CWC_PACK_END

	//! @brief enumerator for a section
	using config_section_enumerator = enumerator<config_entry, 0xFD, 0xE0, 0x42, 0xD0, 0x4, 0xC6, 0x52, 0xD6, 0x8B, 0x6E, 0x26, 0x2F, 0xA3, 0x18, 0x65, 0xF5>;

	//! @brief entry in the configuration
	CWC_PACK_BEGIN
	struct config_section final {
		string_view name;//!< @brief name of section
		intrusive_ptr<config_section_enumerator> enumerator;//!< @brief enumerator for the section
	};
	CWC_PACK_END

	//! @brief enumerator for all sections
	using config_sections_enumerator = enumerator<config_section, 0x74, 0xEB, 0x4, 0x63, 0x95, 0x18, 0x56, 0x78, 0x84, 0x38, 0x9B, 0x10, 0x2B, 0x3D, 0xBD, 0xF4>;
}