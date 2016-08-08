
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
		enum class error_family : uint8 {
			bad     = 1,//derive directly from std::exception, almost exclusively mark serious problems
			logic   = 2,//derive from std::logic_error
			runtime = 3 //derive from std::runtime_error
		};

		//exceptions of the standard library: http://en.cppreference.com/w/cpp/error/exception
		enum class error_code : uint32 {
			no_error                   = 0,

			std98_exception            = static_cast<uint32>(~0),

			std98_bad_exception        = static_cast<uint32>(error_family::bad) << 24,
			std98_bad_cast,
			std98_bad_typeid,
			std11_bad_weak_ptr,
			std11_bad_function_call,
			std98_bad_alloc,
			std11_bad_array_new_length,
			//std14_bad_array_length,

			std98_logic_error          = static_cast<uint32>(error_family::logic) << 24,
			std98_length_error,
			std98_domain_error,
			std98_out_of_range,
			std98_invalid_argument,
			//std11_future_error, (will be reported as logic_error with somewhat helpful message)

			std98_runtime_error        = static_cast<uint32>(error_family::runtime) << 24,
			std98_range_error,
			std98_overflow_error,
			std98_underflow_error,
			//std11_system_error, (will be reported as runtime_error with somewhat helpful message)
			std11_ios_base_failure     = static_cast<uint32>(std98_underflow_error) + 2
			//std11_regex_error (will be reported as runtime_error with somewhat helpful message)
		};

		auto current_exception_to_error_code() noexcept -> error_code;

		template<typename Func>
		auto call_and_return_error(Func func) noexcept -> error_code try {
			func();
			return error_code::no_error;
		} catch(...) { return current_exception_to_error_code(); }

		void validate(error_code errc);
	}
}