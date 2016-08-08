
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ios>
#include <new>
#include <functional>
#include <cwc/cwc.hpp>

namespace cwc {
	namespace internal {
		auto current_exception_to_error_code() noexcept -> error_code {
			try {
				auto eptr = std::current_exception();
				if(eptr) std::rethrow_exception(eptr);
			}

			catch(const std::bad_exception &) { return error_code::std98_bad_exception; }
			catch(const std::bad_cast &) { return error_code::std98_bad_cast; }
			catch(const std::bad_typeid &) { return error_code::std98_bad_typeid; }

#ifndef CWC_HAS_NO_BAD_ARRAY_NEW_LENGTH
			catch(const std::bad_array_new_length &) { return error_code::std11_bad_array_new_length; }
#endif
			catch(const std::bad_alloc &) { return error_code::std98_bad_alloc; }
			catch(const std::bad_weak_ptr &) { return error_code::std11_bad_weak_ptr; }
			catch(const std::bad_function_call &) { return error_code::std11_bad_function_call; }
	
			catch(const std::invalid_argument & exc) {
				this_context().cwc$internal$context$error$3(exc.what());
				return error_code::std98_invalid_argument;
			}
			catch(const std::out_of_range & exc) {
				this_context().cwc$internal$context$error$3(exc.what());
				return error_code::std98_out_of_range;
			}
			catch(const std::domain_error & exc) {
				this_context().cwc$internal$context$error$3(exc.what());
				return error_code::std98_domain_error;
			}
			catch(const std::length_error & exc) {
				this_context().cwc$internal$context$error$3(exc.what());
				return error_code::std98_length_error;
			}
			catch(const std::logic_error & exc) {
				this_context().cwc$internal$context$error$3(exc.what());
				return error_code::std98_logic_error;
			}
	
			catch(const std::ios_base::failure & exc) {
				this_context().cwc$internal$context$error$3(exc.what());
				return error_code::std11_ios_base_failure;
			}
			catch(const std::underflow_error & exc) {
				this_context().cwc$internal$context$error$3(exc.what());
				return error_code::std98_underflow_error;
			}
			catch(const std::overflow_error & exc) {
				this_context().cwc$internal$context$error$3(exc.what());
				return error_code::std98_overflow_error;
			}
			catch(const std::range_error & exc) {
				this_context().cwc$internal$context$error$3(exc.what());
				return error_code::std98_range_error;
			}
			catch(const std::runtime_error & exc) {
				this_context().cwc$internal$context$error$3(exc.what());
				return error_code::std98_runtime_error;
			}
	
			catch(const std::exception & exc) {
				this_context().cwc$internal$context$error$3(exc.what());//handle custom directly from std::exception derived exceptions => preserve message
				return error_code::std98_exception;
			}
			catch(...) { return error_code::std98_exception; }
			return error_code::no_error;
		}

		void validate(error_code errc) {
			if(errc == error_code::no_error) return;

			struct exception : std::exception {//handle case of custom directly from std::exception derived exception => preserve message
				exception(const char * msg) try : msg{msg} {} catch(...) { throw std::exception{}; } //if copying of message fails => force thrown exception to be std::exception
				auto what() const noexcept -> const char * override { return msg.data(); }
			private:
				std::string msg;
			};

			const char * msg{nullptr};
			if(this_context().cwc$internal$context$error$4(&msg) != error_code::no_error) msg = nullptr;//no reliable way to handle such an error!!
			switch(errc) {
				case error_code::no_error: return;
				case error_code::std98_logic_error: throw std::logic_error{msg};
				case error_code::std98_length_error: throw std::length_error{msg};
				case error_code::std98_domain_error: throw std::domain_error{msg};
				case error_code::std98_out_of_range: throw std::out_of_range{msg};
				case error_code::std98_invalid_argument: throw std::invalid_argument{msg};

				case error_code::std98_runtime_error: throw std::runtime_error{msg};
				case error_code::std98_range_error: throw std::range_error{msg};
				case error_code::std98_overflow_error: throw std::overflow_error{msg};
				case error_code::std98_underflow_error: throw std::underflow_error{msg};
				case error_code::std11_ios_base_failure: throw std::ios_base::failure{msg};

				case error_code::std98_bad_exception: throw std::bad_exception{};
				case error_code::std98_bad_cast: throw std::bad_cast{};
				case error_code::std98_bad_typeid: throw std::bad_typeid{};
				case error_code::std11_bad_array_new_length:
#ifndef CWC_HAS_NO_BAD_ARRAY_NEW_LENGTH//fallback to bad_alloc
					throw std::bad_array_new_length{};
#endif
				case error_code::std98_bad_alloc: throw std::bad_alloc{};
				case error_code::std11_bad_weak_ptr: throw std::bad_weak_ptr{};
				case error_code::std11_bad_function_call: throw std::bad_function_call{};
				case error_code::std98_exception: throw msg ? exception{msg} : std::exception{};
				default: {//if error_code code is unknown => extract error_code code and throw families error
					const auto family = static_cast<error_family>(static_cast<uint32>(errc) >> 24);
					switch(family) {
						case error_family::logic: throw std::logic_error{msg};
						case error_family::runtime: throw std::runtime_error{msg};
						case error_family::bad://none of these carries a msg (except for custom derived ones)
						default: //unknown code => throw exception as this is what CWC will always do
							throw exception{msg};
					}
				}
			}
		}
	}
}