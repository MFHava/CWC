
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define WIN32_LEAN_AND_MEAN//suppress #define interface
#include <boost/test/unit_test.hpp>
#include "cwc/cwc.hpp"

using namespace std::string_literals;
using namespace cwc::literals;

BOOST_AUTO_TEST_SUITE(string_view)

BOOST_AUTO_TEST_CASE(size) {
	const std::string s0{"TEST"};
	const cwc::string_view v0{"TEST"};
	BOOST_TEST(s0.size() == v0.size());

	const auto s1 = "TEST"s;
	const auto v1 = "TEST"_sv;
	BOOST_TEST(s1.size() == v1.size());

	const auto s2 = "TEST"s;
	const auto v2 = "TEST"_sv;
	BOOST_TEST(std::strlen(s2.c_str()) == std::strlen(v2.c_str()));
}

BOOST_AUTO_TEST_CASE(empty) {
	const std::string s0;
	const cwc::string_view v0;
	BOOST_TEST(s0.empty());
	BOOST_TEST(v0.empty());

	const std::string s1{""};
	const cwc::string_view v1{""};
	BOOST_TEST(s1.empty());
	BOOST_TEST(v1.empty());

	const auto s2 = ""s;
	const auto v2 = ""_sv;
	BOOST_TEST(s2.empty());
	BOOST_TEST(v2.empty());
}

BOOST_AUTO_TEST_SUITE_END()