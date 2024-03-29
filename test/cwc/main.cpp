
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <any>
#include <regex>
#include <future>
#include <variant>
#include <optional>
#include <filesystem>
#include <functional>

#include "test.cwch"

#ifndef CWC_TEST_DLL
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE("cwc unavailable", "[context]") {
	REQUIRE(!cwc::available<cwc::test::unavailable>());
	REQUIRE_THROWS(cwc::test::unavailable{});
}

TEST_CASE("cwc available", "[context]") {
	REQUIRE(cwc::available<cwc::test::available>());
	REQUIRE_NOTHROW(cwc::test::available{});
}

TEST_CASE("cwc exceptions", "[exceptions]") { //TODO: future_error, regex_error, ios::failure___stream
	cwc::test::available a;
	REQUIRE_NOTHROW(a(0));
	REQUIRE_THROWS_AS(a(1), std::bad_optional_access);
	REQUIRE_THROWS_AS(a(2), std::bad_variant_access);
	REQUIRE_THROWS_AS(a(3), std::bad_function_call);
	REQUIRE_THROWS_AS(a(4), std::bad_weak_ptr);
	REQUIRE_THROWS_AS(a(5), std::bad_exception);
	REQUIRE_THROWS_AS(a(6), std::bad_array_new_length);
	REQUIRE_THROWS_AS(a(7), std::bad_alloc);
	REQUIRE_THROWS_AS(a(8), std::bad_any_cast);
	REQUIRE_THROWS_AS(a(9), std::bad_cast);
	REQUIRE_THROWS_AS(a(10), std::bad_typeid);
	REQUIRE_THROWS_AS(a(11), std::ios_base::failure);
	REQUIRE_THROWS_AS(a(12), std::underflow_error);
	REQUIRE_THROWS_AS(a(13), std::overflow_error);
	REQUIRE_THROWS_AS(a(14), std::range_error);
	REQUIRE_THROWS_AS(a(15), std::runtime_error);
	REQUIRE_THROWS_AS(a(16), std::out_of_range);
	REQUIRE_THROWS_AS(a(17), std::length_error);
	REQUIRE_THROWS_AS(a(18), std::domain_error);
	REQUIRE_THROWS_AS(a(19), std::invalid_argument);
	REQUIRE_THROWS_AS(a(20), std::logic_error);
	REQUIRE_THROWS_AS(a(21), std::exception);
	REQUIRE_THROWS_AS(a(22), std::exception);
}

#else
namespace {
	struct impl final {
		void operator()(int val) {
			switch(val) {
				case 1: throw std::bad_optional_access{};
				case 2: throw std::bad_variant_access{};
				case 3: throw std::bad_function_call{};
				case 4: throw std::bad_weak_ptr{};
				case 5: throw std::bad_exception{};
				case 6: throw std::bad_array_new_length{};
				case 7: throw std::bad_alloc{};
				case 8: throw std::bad_any_cast{};
				case 9: throw std::bad_cast{};
				case 10: throw std::bad_typeid{};
				case 11: throw std::ios_base::failure{""};
				case 12: throw std::underflow_error{""};
				case 13: throw std::overflow_error{""};
				case 14: throw std::range_error{""};
				case 15: throw std::runtime_error{""};
				case 16: throw std::out_of_range{""};
				case 17: throw std::length_error{""};
				case 18: throw std::domain_error{""};
				case 19: throw std::invalid_argument{""};
				case 20: throw std::logic_error{""};
				case 21: throw std::exception{};
				case 22: throw 0;
			}
		}
	};
}

CWC_EXPORT_3cwc4test9available(impl);
#endif
