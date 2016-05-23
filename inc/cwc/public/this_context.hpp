
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include <string>

namespace cwc {
	namespace this_context {
#if defined(CWC_HOST)
		//! @brief modes of context initialization
		enum class init_mode {
			file,//!< @brief initialize from file
			string//!< @brief initialize from string
		};

		//! @brief initialization of the context
		//! @param[in] mode mode of initialization
		//! @param[in] str string to be used in initialization
		//! @note this function must be called before interacting in any way with the context
		//! @attention this function is only available when building a host application
		//! @attention this function may only be called once
		//! @attention invoking this function multiple times or in parallel is undefined behaviour
		void init(init_mode mode, const char * str);
#endif

		//! @brief get the version information of the context
		//! @returns version information of the context
		auto version() -> cwc::version;

		//! @brief get read-only access to the current configuration
		//! @returns enumerator for all sections
		auto config() -> cwc::config_sections_enumerator;

		//! @brief get read-only access to the current configuration
		//! @param[in] section section in the configuration
		//! @returns enumerator for all key-value-pairs in a section
		auto config(const std::string & section) -> cwc::config_section_enumerator;

		//! @brief get read-only access to the current configuration
		//! @param[in] section section in the configuration
		//! @param[in] key key in the configuration section
		//! @returns value assosiated with parameters
		auto config(const std::string & section, const std::string & key) -> std::string;

		//! @brief test if plugin-component is available
		//! @tparam Component component implemented by the plugin
		//! @param[in] id identifier qualifying the tested implementation
		//! @returns true iff implementation is available
		template<typename Component>
		inline
		auto plugin_available(const plugin_id & id) -> bool { return internal::plugin_available(Component::cwc_component::cwc_fqn(), id); }
	}
}