
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cwc/cwc.hpp>

namespace cwc {
	namespace internal {
		auto plugin_available(const char * fqn, const plugin_id & id) -> bool try {
			component::cwc_interface * ptr{nullptr};
			validate(internal::this_context().cwc$internal$context$create$2(fqn, id, &ptr));
			internal::reference_guard tmp{ptr};
			return true;
		} catch(...) {
			return false;
		}
	}

	namespace this_context {
		auto version() -> cwc::version {
			cwc::version result;
			internal::validate(internal::this_context().cwc$internal$context$version$0(&result));
			return result;
		}

		auto config() -> cwc::config_sections_enumerator {
			cwc::config_sections_enumerator::cwc_interface * enumerator{nullptr};
			internal::validate(internal::this_context().cwc$internal$context$config$5(&enumerator));
			assert(enumerator);
			return enumerator;
		}

		auto config(const std::string & section) -> cwc::config_section_enumerator {
			cwc::config_section_enumerator::cwc_interface * enumerator{nullptr};
			internal::validate(internal::this_context().cwc$internal$context$config$6(section.c_str(), &enumerator));
			assert(enumerator);
			return enumerator;
		}

		auto config(const std::string & section, const std::string & key) -> std::string {
			ascii_string result;
			internal::validate(internal::this_context().cwc$internal$context$config$7(section.c_str(), key.c_str(), &result));
			return result;
		}
	}
}