
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
		template<typename Func>
		auto call_and_return_error(Func func) noexcept -> error_code
			try                                                  { return func(),                            error_code::no_error;                   }
			//NOTE: currently not available: catch(const std::bad_variant_access           &    ) { return                                    error_code::std17_bad_variant_access;   }
			catch(const std::bad_function_call            &    ) { return                                    error_code::std11_bad_function_call;    }
			catch(const std::bad_weak_ptr                 &    ) { return                                    error_code::std11_bad_weak_ptr;         }
			catch(const std::bad_exception                &    ) { return                                    error_code::std98_bad_exception;        }
			catch(const std::bad_array_new_length         &    ) { return                                    error_code::std11_bad_array_new_length; }
			catch(const std::bad_alloc                    &    ) { return                                    error_code::std98_bad_alloc;            }
			//NOTE: currently not available: catch(const std::bad_any_cast                 &    ) { return                                    error_code::std17_bad_any_cast;         }
			catch(const std::bad_cast                     &    ) { return                                    error_code::std98_bad_cast;             }
			catch(const std::bad_typeid                   &    ) { return                                    error_code::std98_bad_typeid;           }
			//NOTE: currently not available: catch(const std::filesystem::filesystem_error & exc) { return this_context()->error(exc.what()), error_code::std17_filesystem_error;     }
			catch(const std::ios_base::failure            & exc) { return this_context()->error(exc.what()), error_code::std11_ios_base_failure;     }
			//NOTE: not constructable from msg: catch(const std::system_error                 &    ) { return                                    error_code::std11_system_error;         }
			//NOTE: not constructable from msg: catch(const std::regex_error                  &    ) { return                                    error_code::std11_regex_error;          }
			catch(const std::underflow_error              & exc) { return this_context()->error(exc.what()), error_code::std98_underflow_error;      }
			catch(const std::overflow_error               & exc) { return this_context()->error(exc.what()), error_code::std98_overflow_error;       }
			catch(const std::range_error                  & exc) { return this_context()->error(exc.what()), error_code::std98_range_error;          }
			catch(const std::runtime_error                & exc) { return this_context()->error(exc.what()), error_code::std98_runtime_error;        }
			//NOTE: currently not available: catch(const std::bad_optional_access          &    ) { return                                    error_code::std17_bad_optional_access;  }
			//NOTE: not constructable from msg: catch(const std::future_error                 &    ) { return                                    error_code::std11_future_error;         }
			catch(const std::out_of_range                 & exc) { return this_context()->error(exc.what()), error_code::std98_out_of_range;         }
			catch(const std::length_error                 & exc) { return this_context()->error(exc.what()), error_code::std98_length_error;         }
			catch(const std::domain_error                 & exc) { return this_context()->error(exc.what()), error_code::std98_domain_error;         }
			catch(const std::invalid_argument             & exc) { return this_context()->error(exc.what()), error_code::std98_invalid_argument;     }
			catch(const std::logic_error                  & exc) { return this_context()->error(exc.what()), error_code::std98_logic_error;          }
			catch(const std::exception                    & exc) { return this_context()->error(exc.what()), error_code::std98_exception;            }
			catch(                                          ...) { return                                    error_code::std98_exception;            }

		inline
		void validate(error_code code) {
			if(code == error_code::no_error) return;

			struct unknown_error : std::exception {
				unknown_error() {}
				auto what() const noexcept -> const char * override { return "received unknown error code"; }
			};

			using UI = std::underlying_type<error_code>::type;

			for(auto mask : {UI{0}, UI{127}, UI{255} << 7, UI{255} << 15, UI{255} << 23})
				switch(code = static_cast<error_code>(~mask & static_cast<UI>(code))) {//slice inheritance level
					case error_code::no_error:                   throw unknown_error{};//slicing removed all bits => does not match definition of exceptions!
					case error_code::std98_exception:            throw std::exception{};
					case error_code::std98_logic_error:          throw std::logic_error{this_context()->error().c_str()};
					case error_code::std98_invalid_argument:     throw std::invalid_argument{this_context()->error().c_str()};
					case error_code::std98_domain_error:         throw std::domain_error{this_context()->error().c_str()};
					case error_code::std98_length_error:         throw std::length_error{this_context()->error().c_str()};
					case error_code::std98_out_of_range:         throw std::out_of_range{this_context()->error().c_str()};
					//NOTE: not constructable from msg: case error_code::std11_future_error:
					//NOTE: currently not available: case error_code::std17_bad_optional_access:
					case error_code::std98_runtime_error:        throw std::runtime_error{this_context()->error().c_str()};
					case error_code::std98_range_error:          throw std::range_error{this_context()->error().c_str()};
					case error_code::std98_overflow_error:       throw std::overflow_error{this_context()->error().c_str()};
					case error_code::std98_underflow_error:      throw std::underflow_error{this_context()->error().c_str()};
					//not constructable from msg: case error_code::std11_regex_error:
					//not constructable from msg: case error_code::std11_system_error:
					case error_code::std11_ios_base_failure:     throw std::ios_base::failure{this_context()->error().c_str()};
					//NOTE: currently not available: case error_code::std17_filesystem_error:
					case error_code::std98_bad_typeid:           throw std::bad_typeid{};
					case error_code::std98_bad_cast:             throw std::bad_cast{};
					//NOTE: currently not available: case error_code::std17_bad_any_cast:
					case error_code::std98_bad_alloc:            throw std::bad_alloc{};
					case error_code::std11_bad_array_new_length: throw std::bad_array_new_length{};
					case error_code::std98_bad_exception:        throw std::bad_exception{};
					case error_code::std11_bad_weak_ptr:         throw std::bad_weak_ptr{};
					case error_code::std11_bad_function_call:    throw std::bad_function_call{};
					//NOTE: currently not available: case error_code::std17_bad_variant_access:
				}
			throw unknown_error{};
		}
	}
}