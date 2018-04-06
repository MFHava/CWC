
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
		template<uint8 Level0 = 0, uint8 Level1 = 0, uint8 Level2 = 0, uint8 Level3 = 0, uint8 Level4 = 0, uint8 Level5 = 0, uint8 Level6 = 0, uint8 Level7 = 0>
		struct error_code_helper final : std::integral_constant<uint64, (static_cast<uint64>(Level0) << 56) | (static_cast<uint64>(Level1) << 48) | (static_cast<uint64>(Level2) << 40) | (static_cast<uint64>(Level3) << 32) | (static_cast<uint64>(Level4) << 24) | (static_cast<uint64>(Level5) << 16) | (static_cast<uint64>(Level6) << 8) | static_cast<uint64>(Level7)> {};

		//exceptions of the standard library: http://en.cppreference.com/w/cpp/error/exception
		enum class error_code : uint64 {
			std98_exception = error_code_helper<>::value,
				std98_logic_error = error_code_helper<1>::value,
					std98_invalid_argument = error_code_helper<1, 1>::value,
					std98_domain_error = error_code_helper<1, 2>::value,
					std98_length_error = error_code_helper<1, 3>::value,
					std98_out_of_range = error_code_helper<1, 4>::value,
					std11_future_error = error_code_helper<1, 5>::value,
				std98_runtime_error = error_code_helper<2>::value,
					std98_range_error = error_code_helper<2, 1>::value,
					std98_overflow_error = error_code_helper<2, 2>::value,
					std98_underflow_error = error_code_helper<2, 3>::value,
					std11_regex_error = error_code_helper<2, 4>::value,
					std11_system_error = error_code_helper<2, 5>::value,
						std11_ios_base_failure = error_code_helper<2, 5, 1>::value,
						std17_filesystem_error = error_code_helper<2, 5, 2>::value,
				std98_bad_typeid = error_code_helper<3>::value,
				std98_bad_cast = error_code_helper<4>::value,
					std17_bad_any_cast = error_code_helper<4, 1>::value,
				std98_bad_alloc = error_code_helper<5>::value,
					std11_bad_array_new_length = error_code_helper<5, 1>::value,
				std98_bad_exception = error_code_helper<6>::value,
				std11_bad_weak_ptr = error_code_helper<7>::value,
				std11_bad_function_call = error_code_helper<8>::value,
				std17_bad_variant_access = error_code_helper<9>::value,
				std17_bad_optional_access = error_code_helper<10>::value,
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
}
