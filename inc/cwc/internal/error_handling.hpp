
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include <memory>

namespace cwc {
	namespace internal {
		auto store_exception(std::exception_ptr eptr) noexcept -> const error *
			try { std::rethrow_exception(eptr); }
			//NOTE: currently not available: catch(const std::bad_optional_access & exc) { return this_context()->exception({error_code::std17_bad_optional_access, exc.what()}); }
			//NOTE: currently not available: catch(const std::bad_variant_access & exc) { return this_context()->exception({error_code::std17_bad_variant_access, exc.what()}); }
			catch(const std::bad_function_call & exc) { return this_context()->exception({error_code::std11_bad_function_call, exc.what()}); }
			catch(const std::bad_weak_ptr & exc) { return this_context()->exception({error_code::std11_bad_weak_ptr, exc.what()}); }
			catch(const std::bad_exception & exc) { return this_context()->exception({error_code::std98_bad_exception, exc.what()}); }
			catch(const std::bad_array_new_length & exc) { return this_context()->exception({error_code::std11_bad_array_new_length, exc.what()}); }
			catch(const std::bad_alloc & exc) { return this_context()->exception({error_code::std98_bad_alloc, exc.what()}); }
			//NOTE: currently not available: catch(const std::bad_any_cast & exc) { return this_context()->exception({error_code::std17_bad_any_cast, exc.what()}); }
			catch(const std::bad_cast & exc) { return this_context()->exception({error_code::std98_bad_cast, exc.what()}); }
			catch(const std::bad_typeid & exc) { return this_context()->exception({error_code::std98_bad_typeid, exc.what()}); }
			//NOTE: currently not available: catch(const std::filesystem::filesystem_error & exc) { return this_context()->exception({error_code::std17_filesystem_error, exc.what()}); }
			catch(const std::ios_base::failure & exc) { return this_context()->exception({error_code::std11_ios_base_failure, exc.what()}); }
			//NOTE: not constructable from msg: catch(const std::system_error & exc) { return this_context()->exception({error_code::std11_system_error, exc.what()}); }
			//NOTE: not constructable from msg: catch(const std::regex_error & exc) { return this_context()->exception({error_code::std11_regex_error, exc.what()}); }
			catch(const std::underflow_error & exc) { return this_context()->exception({error_code::std98_underflow_error, exc.what()}); }
			catch(const std::overflow_error & exc) { return this_context()->exception({error_code::std98_overflow_error, exc.what()}); }
			catch(const std::range_error & exc) { return this_context()->exception({error_code::std98_range_error, exc.what()}); }
			catch(const std::runtime_error & exc) { return this_context()->exception({error_code::std98_runtime_error, exc.what()}); }
			//NOTE: not constructable from msg: catch(const std::future_error & exc) { return this_context()->exception({error_code::std11_future_error, exc.what()}); }
			catch(const std::out_of_range & exc) { return this_context()->exception({error_code::std98_out_of_range, exc.what()}); }
			catch(const std::length_error & exc) { return this_context()->exception({error_code::std98_length_error, exc.what()}); }
			catch(const std::domain_error & exc) { return this_context()->exception({error_code::std98_domain_error, exc.what()}); }
			catch(const std::invalid_argument & exc) { return this_context()->exception({error_code::std98_invalid_argument, exc.what()}); }
			catch(const std::logic_error & exc) { return this_context()->exception({error_code::std98_logic_error, exc.what()}); }
			catch(const std::exception & exc) { return this_context()->exception({error_code::std98_exception, exc.what()}); }
			catch(...) { return this_context()->exception({error_code::std98_exception, ""}); }

		void validate(const error * err) {
			if(!err) return;

			struct unknown_error : std::exception {
				unknown_error() {}
				auto what() const noexcept -> const char * override { return "received unknown error code"; }
			};

			using UI = std::underlying_type<error_code>::type;

			auto code{err->code};
			for(auto mask : {UI{0}, UI{255}, UI{255} << 8, UI{255} << 16, UI{255} << 24, UI{255} << 32, UI{255} << 40, UI{255} << 48, UI{255} << 56})
				switch(code = static_cast<error_code>(~mask & static_cast<UI>(code))) {//slice inheritance level
					case error_code::std98_exception: throw std::exception{};
					case error_code::std98_logic_error: throw std::logic_error{err->message};
					case error_code::std98_invalid_argument: throw std::invalid_argument{err->message};
					case error_code::std98_domain_error: throw std::domain_error{err->message};
					case error_code::std98_length_error: throw std::length_error{err->message};
					case error_code::std98_out_of_range: throw std::out_of_range{err->message};
					//NOTE: not constructable from msg: case error_code::std11_future_error:
					case error_code::std98_runtime_error: throw std::runtime_error{err->message};
					case error_code::std98_range_error: throw std::range_error{err->message};
					case error_code::std98_overflow_error: throw std::overflow_error{err->message};
					case error_code::std98_underflow_error: throw std::underflow_error{err->message};
					//not constructable from msg: case error_code::std11_regex_error:
					//not constructable from msg: case error_code::std11_system_error:
					case error_code::std11_ios_base_failure: throw std::ios_base::failure{err->message};
					//NOTE: currently not available: case error_code::std17_filesystem_error:
					case error_code::std98_bad_typeid: throw std::bad_typeid{};
					case error_code::std98_bad_cast: throw std::bad_cast{};
					//NOTE: currently not available: case error_code::std17_bad_any_cast:
					case error_code::std98_bad_alloc: throw std::bad_alloc{};
					case error_code::std11_bad_array_new_length: throw std::bad_array_new_length{};
					case error_code::std98_bad_exception: throw std::bad_exception{};
					case error_code::std11_bad_weak_ptr: throw std::bad_weak_ptr{};
					case error_code::std11_bad_function_call: throw std::bad_function_call{};
					//NOTE: currently not available: case error_code::std17_bad_optional_access:
					//NOTE: currently not available: case error_code::std17_bad_variant_access:
				}
			throw unknown_error{};
		}
	}
}
