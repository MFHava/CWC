
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cwc/cwc.hpp>

namespace cwc {
	config_section_enumerator::config_section_enumerator(cwc_interface * ptr) : component{ptr} {}

	config_section_enumerator::config_section_enumerator(const cwc_interface * ptr) : component{ptr} {}

	auto config_section_enumerator::end() const -> internal::abi<const boolean>::ret {
		internal::abi<const boolean>::param cwc_ret;
		internal::validate(cwc_marshall<cwc_self>()->cwc$config_section_enumerator$end$0(internal::to_abi(cwc_ret)));
		return internal::from_abi(cwc_ret);
	}

	void config_section_enumerator::next() { internal::validate(cwc_marshall<cwc_self>()->cwc$config_section_enumerator$next$1()); }

	auto config_section_enumerator::get() const -> internal::abi<const config_entry>::ret {
		internal::abi<const config_entry>::param cwc_ret;
		internal::validate(cwc_marshall<cwc_self>()->cwc$config_section_enumerator$get$2(internal::to_abi(cwc_ret)));
		return internal::from_abi(cwc_ret);
	}

	config_section_enumerator::config_section_enumerator() {}


	config_sections_enumerator::config_sections_enumerator(cwc_interface * ptr) : component{ptr} {}

	config_sections_enumerator::config_sections_enumerator(const cwc_interface * ptr) : component{ptr} {}

	auto config_sections_enumerator::end() const -> internal::abi<const boolean>::ret {
		internal::abi<const boolean>::param cwc_ret;
		internal::validate(cwc_marshall<cwc_self>()->cwc$config_sections_enumerator$end$0(internal::to_abi(cwc_ret)));
		return internal::from_abi(cwc_ret);
	}

	void config_sections_enumerator::next() { internal::validate(cwc_marshall<cwc_self>()->cwc$config_sections_enumerator$next$1()); }

	auto config_sections_enumerator::get() const -> internal::abi<const ascii_string>::ret {
		internal::abi<const ascii_string>::param cwc_ret;
		internal::validate(cwc_marshall<cwc_self>()->cwc$config_sections_enumerator$get$2(internal::to_abi(cwc_ret)));
		return internal::from_abi(cwc_ret);
	}

	config_sections_enumerator::config_sections_enumerator() {}
}