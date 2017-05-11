
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
		//exceptions of the standard library: http://en.cppreference.com/w/cpp/error/exception
		enum class error_code : uint32 {
			no_error,
			std98_exception                    = static_cast<uint32>(1) << 31,
				std98_logic_error              = +std98_exception | static_cast<uint32>(1) << 23,
					std98_invalid_argument     = 	+std98_logic_error | static_cast<uint32>(1) << 15,
					std98_domain_error         = 	+std98_logic_error | static_cast<uint32>(2) << 15,
					std98_length_error         = 	+std98_logic_error | static_cast<uint32>(3) << 15,
					std98_out_of_range         = 	+std98_logic_error | static_cast<uint32>(4) << 15,
					std11_future_error         = 	+std98_logic_error | static_cast<uint32>(5) << 15,
					std17_bad_optional_access  = 	+std98_logic_error | static_cast<uint32>(6) << 15,
				std98_runtime_error            = +std98_exception | static_cast<uint32>(2) << 23,
					std98_range_error          = 	+std98_runtime_error | static_cast<uint32>(1) << 15,
					std98_overflow_error       = 	+std98_runtime_error | static_cast<uint32>(2) << 15,
					std98_underflow_error      = 	+std98_runtime_error | static_cast<uint32>(3) << 15,
					std11_regex_error          = 	+std98_runtime_error | static_cast<uint32>(4) << 15,
					std11_system_error         = 	+std98_runtime_error | static_cast<uint32>(5) << 15,
						std11_ios_base_failure = 		+std11_system_error | static_cast<uint32>(1) << 7,
						std17_filesystem_error = 		+std11_system_error | static_cast<uint32>(2) << 7,
				std98_bad_typeid               = +std98_exception | static_cast<uint32>(3) << 23,
				std98_bad_cast                 = +std98_exception | static_cast<uint32>(4) << 23,
					std17_bad_any_cast         = 	+std98_bad_cast | static_cast<uint32>(1) << 15,
				std98_bad_alloc                = +std98_exception | static_cast<uint32>(5) << 23,
					std11_bad_array_new_length = 	+std98_bad_alloc | 1 << 15,
				std98_bad_exception            = +std98_exception | static_cast<uint32>(6) << 23,
				std11_bad_weak_ptr             = +std98_exception | static_cast<uint32>(7) << 23,
				std11_bad_function_call        = +std98_exception | static_cast<uint32>(8) << 23,
				std17_bad_variant_access       = +std98_exception | static_cast<uint32>(9) << 23,
		};

		auto store_exception(std::exception_ptr eptr) noexcept -> error_code;
		void validate(error_code code);

		template<typename Func>
		auto call_and_return_error(Func func) noexcept -> error_code
			try { return func(), error_code::no_error; }
			catch(...) { return store_exception(std::current_exception()); }
	}
}