
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define WIN32_LEAN_AND_MEAN//suppress #define interface
#include <boost/test/unit_test.hpp>
#include "cwc/cwc.hpp"

BOOST_AUTO_TEST_SUITE(error_handling)
BOOST_AUTO_TEST_CASE(no_error) { BOOST_TEST((cwc::internal::call_and_return_error([] {}) == cwc::internal::error_code::no_error)); }

template<typename Exception>
void test(const Exception & exc, cwc::internal::error_code errc) {
	const auto code = cwc::internal::call_and_return_error([&] { throw exc; });;
	BOOST_TEST((code == errc));
}

BOOST_AUTO_TEST_CASE(exception          ) { test(std::exception{},             cwc::internal::error_code::std98_exception           ); }

BOOST_AUTO_TEST_CASE(logic_error        ) { test(std::logic_error{""},         cwc::internal::error_code::std98_logic_error         ); }
BOOST_AUTO_TEST_CASE(invalid_argument   ) { test(std::invalid_argument{""},    cwc::internal::error_code::std98_invalid_argument    ); }
BOOST_AUTO_TEST_CASE(domain_error       ) { test(std::domain_error{""},        cwc::internal::error_code::std98_domain_error        ); }
BOOST_AUTO_TEST_CASE(length_error       ) { test(std::length_error{""},        cwc::internal::error_code::std98_length_error        ); }
BOOST_AUTO_TEST_CASE(out_of_range       ) { test(std::out_of_range{""},        cwc::internal::error_code::std98_out_of_range        ); }
//NOTE: not constructable from msg: std11_future_error
//NOTE: currently not available: std17_bad_optional_access

BOOST_AUTO_TEST_CASE(runtime_error      ) { test(std::runtime_error{""},       cwc::internal::error_code::std98_runtime_error       ); }
BOOST_AUTO_TEST_CASE(range_error        ) { test(std::range_error{""},         cwc::internal::error_code::std98_range_error         ); }
BOOST_AUTO_TEST_CASE(overflow_error     ) { test(std::overflow_error{""},      cwc::internal::error_code::std98_overflow_error      ); }
BOOST_AUTO_TEST_CASE(underflow_error    ) { test(std::underflow_error{""},     cwc::internal::error_code::std98_underflow_error     ); }
//NOTE: not constructable from msg: std11_regex_error
//NOTE: not constructable from msg: std11_system_error
BOOST_AUTO_TEST_CASE(ios_base_failure   ) { test(std::ios_base::failure{""},   cwc::internal::error_code::std11_ios_base_failure    ); }
//NOTE: currently not available: std17_filesystem_error

BOOST_AUTO_TEST_CASE(bad_typeid          ) { test(std::bad_typeid{},           cwc::internal::error_code::std98_bad_typeid          ); }
BOOST_AUTO_TEST_CASE(bad_cast            ) { test(std::bad_cast{},             cwc::internal::error_code::std98_bad_cast            ); }
//NOTE: currently not available: std17_bad_any_cast
BOOST_AUTO_TEST_CASE(bad_alloc           ) { test(std::bad_alloc{},            cwc::internal::error_code::std98_bad_alloc           ); }
BOOST_AUTO_TEST_CASE(bad_array_new_length) { test(std::bad_array_new_length{}, cwc::internal::error_code::std11_bad_array_new_length); }
BOOST_AUTO_TEST_CASE(bad_exception       ) { test(std::bad_exception{},        cwc::internal::error_code::std98_bad_exception       ); }
BOOST_AUTO_TEST_CASE(bad_weak_ptr        ) { test(std::bad_weak_ptr{},         cwc::internal::error_code::std11_bad_weak_ptr        ); }
BOOST_AUTO_TEST_CASE(bad_function_call   ) { test(std::bad_function_call{},    cwc::internal::error_code::std11_bad_function_call   ); }
//NOTE: currently not available: std17_bad_variant_access

//TODO: test inherited exceptions (slicing of error code)



BOOST_AUTO_TEST_SUITE_END()