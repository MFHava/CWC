
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc::internal {
	template<uint8 Level0 = 0, uint8 Level1 = 0, uint8 Level2 = 0, uint8 Level3 = 0, uint8 Level4 = 0, uint8 Level5 = 0, uint8 Level6 = 0, uint8 Level7 = 0>
	struct error_code_helper final : std::integral_constant<uint64, (static_cast<uint64>(Level0) << 56) | (static_cast<uint64>(Level1) << 48) | (static_cast<uint64>(Level2) << 40) | (static_cast<uint64>(Level3) << 32) | (static_cast<uint64>(Level4) << 24) | (static_cast<uint64>(Level5) << 16) | (static_cast<uint64>(Level6) << 8) | static_cast<uint64>(Level7)> {};

	template<uint8... Levels>
	inline
	constexpr
	auto error_code_helper_v{error_code_helper<Levels...>::value};

	//exceptions of the standard library: http://en.cppreference.com/w/cpp/error/exception
	enum class error_code : uint64 {
		std98_exception = error_code_helper_v<>,
			std98_logic_error = error_code_helper_v<1>,
				std98_invalid_argument = error_code_helper_v<1, 1>,
				std98_domain_error = error_code_helper_v<1, 2>,
				std98_length_error = error_code_helper_v<1, 3>,
				std98_out_of_range = error_code_helper_v<1, 4>,
				std11_future_error = error_code_helper_v<1, 5>,
			std98_runtime_error = error_code_helper_v<2>,
				std98_range_error = error_code_helper_v<2, 1>,
				std98_overflow_error = error_code_helper_v<2, 2>,
				std98_underflow_error = error_code_helper_v<2, 3>,
				std11_regex_error = error_code_helper_v<2, 4>,
				std11_system_error = error_code_helper_v<2, 5>,
					std11_ios_base_failure = error_code_helper_v<2, 5, 1>,
					std17_filesystem_error = error_code_helper_v<2, 5, 2>,
			std98_bad_typeid = error_code_helper_v<3>,
			std98_bad_cast = error_code_helper_v<4>,
				std17_bad_any_cast = error_code_helper_v<4, 1>,
			std98_bad_alloc = error_code_helper_v<5>,
				std11_bad_array_new_length = error_code_helper_v<5, 1>,
			std98_bad_exception = error_code_helper_v<6>,
			std11_bad_weak_ptr = error_code_helper_v<7>,
			std11_bad_function_call = error_code_helper_v<8>,
			std17_bad_variant_access = error_code_helper_v<9>,
			std17_bad_optional_access = error_code_helper_v<10>,
	};

	CWC_PACK_BEGIN
	struct error final {
		cwc::internal::error_code code;
		const cwc::utf8 * message{""};
	};
	CWC_PACK_END

	auto store_exception(std::exception_ptr eptr) noexcept -> const error *;
	void validate(const error * err);

	template<typename Func>
	auto call_and_return_error(Func func) noexcept -> const error *
		try { return func(), nullptr; }
		catch(...) { return store_exception(std::current_exception()); }
	
}
