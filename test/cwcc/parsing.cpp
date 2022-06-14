
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <catch.hpp>
#include "ast.hpp"
#include "parser.hpp"

TEST_CASE("parsing_attribute_list", "[parsing] [attribute_list]") {
	auto attribute_list{[](const char * str) {
		std::stringstream ss{str};
		cwcc::parser p{ss};
		return cwcc::attribute_list{p};
	}};

	REQUIRE(attribute_list("") == cwcc::attribute_list{{}, {}});
	REQUIRE(attribute_list("[[deprecated]]") == cwcc::attribute_list{"", {}});
	REQUIRE(attribute_list("[[nodiscard]]") == cwcc::attribute_list{{}, ""});
	REQUIRE(attribute_list("[[deprecated, nodiscard]]") == cwcc::attribute_list{"", ""});
	REQUIRE(attribute_list("[[nodiscard, deprecated]]") == cwcc::attribute_list{"", ""});
	REQUIRE(attribute_list("[[deprecated]][[nodiscard]]") == cwcc::attribute_list{"", ""});
	REQUIRE(attribute_list("[[nodiscard]][[deprecated]]") == cwcc::attribute_list{"", ""});

	REQUIRE(attribute_list("[[nodiscard(\"allocates memory\")]]") == cwcc::attribute_list{{}, "\"allocates memory\""});
	REQUIRE(attribute_list("[[nodiscard(\"allocates memory\")]][[deprecated(\"outdated interface\")]]") == cwcc::attribute_list{"\"outdated interface\"", "\"allocates memory\""});

	REQUIRE_THROWS(attribute_list("[[nodiscard, nodiscard]][[deprecated]]"));
	REQUIRE_THROWS(attribute_list("[[nodiscard]][[deprecated, deprecated]]"));
	REQUIRE_THROWS(attribute_list("[[nodiscard, deprecated]][[nodiscard]]"));
	REQUIRE_THROWS(attribute_list("[[nodiscard, deprecated]][[deprecated]]"));
}

TEST_CASE("parsing_param", "[parsing] [param]") {
	auto param{[](const char * str) {
		std::stringstream ss{str};
		cwcc::parser p{ss};
		return cwcc::param{p};
	}};

	REQUIRE(param("int val") == cwcc::param{false, "int", cwcc::ref_t::none, "val", ""});
	REQUIRE(param("int val //!< test test") == cwcc::param{false, "int", cwcc::ref_t::none, "val", "//!< test test"});
	REQUIRE(param("int & val") == cwcc::param{false, "int", cwcc::ref_t::lvalue, "val", ""});
	REQUIRE(param("int & val //!< test test") == cwcc::param{false, "int", cwcc::ref_t::lvalue, "val", "//!< test test"});
	REQUIRE(param("int && val") == cwcc::param{false, "int", cwcc::ref_t::rvalue, "val", ""});
	REQUIRE(param("int && val //!< test test") == cwcc::param{false, "int", cwcc::ref_t::rvalue, "val", "//!< test test"});
	REQUIRE(param("const int & val") == cwcc::param{true, "int", cwcc::ref_t::lvalue, "val", ""});
	REQUIRE(param("const int & val //!< test test") == cwcc::param{true, "int", cwcc::ref_t::lvalue, "val", "//!< test test"});

	REQUIRE_THROWS(param("const int val"));
	REQUIRE_THROWS(param("const int val //!< test test"));
}

TEST_CASE("parsing_param_list", "[parsing] [param_list]") {
	auto param_list{[](const char * str) {
		std::stringstream ss{str};
		cwcc::parser p{ss};
		return cwcc::param_list{p};
	}};

	REQUIRE(param_list("int i") == cwcc::param_list{{{false, "int", cwcc::ref_t::none, "i", ""}}});
	REQUIRE(param_list("int i, double d") == cwcc::param_list{{{false, "int", cwcc::ref_t::none, "i", ""}, {false, "double", cwcc::ref_t::none, "d", ""}}});
}

TEST_CASE("parsing_comment_list", "[parsing] [comment_list]") {
	auto comment_list{[](const char * str) {
		std::stringstream ss{str};
		cwcc::parser p{ss};
		return cwcc::comment_list{p};
	}};

	REQUIRE(comment_list("") == cwcc::comment_list{{}});
	REQUIRE(comment_list("//test") == cwcc::comment_list{{"//test"}});
	REQUIRE(comment_list("//test\n//test2") == cwcc::comment_list{{"//test", "//test2"}});
	REQUIRE(comment_list("//test\n//test2\n//this is another test") == cwcc::comment_list{{"//test", "//test2", "//this is another test"}});

	REQUIRE_THROWS(comment_list("*"));
	REQUIRE_THROWS(comment_list("/*"));
}

TEST_CASE("parsing_constructors", "[parsing] [constructors]") {
	auto constructor{[](const char * str) {
		std::stringstream ss{str};
		cwcc::parser p{ss};
		return cwcc::constructor{p, {}, {}};
	}};

	REQUIRE(constructor("()") == cwcc::constructor{{}, false});
	REQUIRE(constructor("(int val)") == cwcc::constructor{cwcc::param_list{{{false, "int", cwcc::ref_t::none, "val", ""}}}, false});

	REQUIRE(constructor("() =delete") == cwcc::constructor{{}, true});
	REQUIRE(constructor("(int val) =delete") == cwcc::constructor{cwcc::param_list{{{false, "int", cwcc::ref_t::none, "val", ""}}}, true});

	REQUIRE_THROWS(constructor("() =default"));
	REQUIRE_THROWS(constructor("(int val) =default"));
}

TEST_CASE("parsing_method", "[parsing] [method]") {
	auto method{[](const char * str) {
		std::stringstream ss{str};
		cwcc::parser p{ss};
		return cwcc::method{p, {}, {}};
	}};

	REQUIRE(method("void func()") == cwcc::method{false, "func", {}, false, cwcc::ref_t::none, false, {}, false});
	REQUIRE(method("void func() const") == cwcc::method{false, "func", {}, true, cwcc::ref_t::none, false, {}, false});
	REQUIRE(method("void func() const noexcept") == cwcc::method{false, "func", {}, true, cwcc::ref_t::none, true, {}, false});
	REQUIRE(method("void func()       noexcept") == cwcc::method{false, "func", {}, false, cwcc::ref_t::none, true, {}, false});

	REQUIRE(method("void func()       &") == cwcc::method{false, "func", {}, false, cwcc::ref_t::lvalue, false, {}, false});
	REQUIRE(method("void func() const &") == cwcc::method{false, "func", {}, true, cwcc::ref_t::lvalue, false, {}, false});
	REQUIRE(method("void func() const & noexcept") == cwcc::method{false, "func", {}, true, cwcc::ref_t::lvalue, true, {}, false});
	REQUIRE(method("void func()       & noexcept") == cwcc::method{false, "func", {}, false, cwcc::ref_t::lvalue, true, {}, false});

	REQUIRE(method("void func()       &&") == cwcc::method{false, "func", {}, false, cwcc::ref_t::rvalue, false, {}, false});
	REQUIRE(method("void func() const &&") == cwcc::method{false, "func", {}, true, cwcc::ref_t::rvalue, false, {}, false});
	REQUIRE(method("void func() const && noexcept") == cwcc::method{false, "func", {}, true, cwcc::ref_t::rvalue, true, {}, false});
	REQUIRE(method("void func()       && noexcept") == cwcc::method{false, "func", {}, false, cwcc::ref_t::rvalue, true, {}, false});


	REQUIRE(method("auto func()                -> int") == cwcc::method{false, "func", {}, false, cwcc::ref_t::none, false, "int", false});
	REQUIRE(method("auto func() const          -> int") == cwcc::method{false, "func", {}, true, cwcc::ref_t::none, false, "int", false});
	REQUIRE(method("auto func() const noexcept -> int") == cwcc::method{false, "func", {}, true, cwcc::ref_t::none, true, "int", false});
	REQUIRE(method("auto func()       noexcept -> int") == cwcc::method{false, "func", {}, false, cwcc::ref_t::none, true, "int", false});

	REQUIRE(method("auto func()       &          -> int") == cwcc::method{false, "func", {}, false, cwcc::ref_t::lvalue, false, "int", false});
	REQUIRE(method("auto func() const &          -> int") == cwcc::method{false, "func", {}, true, cwcc::ref_t::lvalue, false, "int", false});
	REQUIRE(method("auto func() const & noexcept -> int") == cwcc::method{false, "func", {}, true, cwcc::ref_t::lvalue, true, "int", false});
	REQUIRE(method("auto func()       & noexcept -> int") == cwcc::method{false, "func", {}, false, cwcc::ref_t::lvalue, true, "int", false});

	REQUIRE(method("auto func()       &&          -> int") == cwcc::method{false, "func", {}, false, cwcc::ref_t::rvalue, false, "int", false});
	REQUIRE(method("auto func() const &&          -> int") == cwcc::method{false, "func", {}, true, cwcc::ref_t::rvalue, false, "int", false});
	REQUIRE(method("auto func() const && noexcept -> int") == cwcc::method{false, "func", {}, true, cwcc::ref_t::rvalue, true, "int", false});
	REQUIRE(method("auto func()       && noexcept -> int") == cwcc::method{false, "func", {}, false, cwcc::ref_t::rvalue, true, "int", false});
	 
	REQUIRE(method("void func()                =delete") == cwcc::method{false, "func", {}, false, cwcc::ref_t::none, false, {}, true});
	REQUIRE(method("void func() const          =delete") == cwcc::method{false, "func", {}, true, cwcc::ref_t::none, false, {}, true});
	REQUIRE(method("void func() const noexcept =delete") == cwcc::method{false, "func", {}, true, cwcc::ref_t::none, true, {}, true});
	REQUIRE(method("void func()       noexcept =delete") == cwcc::method{false, "func", {}, false, cwcc::ref_t::none, true, {}, true});

	REQUIRE(method("void func()       &          =delete") == cwcc::method{false, "func", {}, false, cwcc::ref_t::lvalue, false, {}, true});
	REQUIRE(method("void func() const &          =delete") == cwcc::method{false, "func", {}, true, cwcc::ref_t::lvalue, false, {}, true});
	REQUIRE(method("void func() const & noexcept =delete") == cwcc::method{false, "func", {}, true, cwcc::ref_t::lvalue, true, {}, true});
	REQUIRE(method("void func()       & noexcept =delete") == cwcc::method{false, "func", {}, false, cwcc::ref_t::lvalue, true, {}, true});

	REQUIRE(method("void func()       &&          =delete") == cwcc::method{false, "func", {}, false, cwcc::ref_t::rvalue, false, {}, true});
	REQUIRE(method("void func() const &&          =delete") == cwcc::method{false, "func", {}, true, cwcc::ref_t::rvalue, false, {}, true});
	REQUIRE(method("void func() const && noexcept =delete") == cwcc::method{false, "func", {}, true, cwcc::ref_t::rvalue, true, {}, true});
	REQUIRE(method("void func()       && noexcept =delete") == cwcc::method{false, "func", {}, false, cwcc::ref_t::rvalue, true, {}, true});


	REQUIRE(method("auto func()                -> int =delete") == cwcc::method{false, "func", {}, false, cwcc::ref_t::none, false, "int", true});
	REQUIRE(method("auto func() const          -> int =delete") == cwcc::method{false, "func", {}, true, cwcc::ref_t::none, false, "int", true});
	REQUIRE(method("auto func() const noexcept -> int =delete") == cwcc::method{false, "func", {}, true, cwcc::ref_t::none, true, "int", true});
	REQUIRE(method("auto func()       noexcept -> int =delete") == cwcc::method{false, "func", {}, false, cwcc::ref_t::none, true, "int", true});

	REQUIRE(method("auto func()       &          -> int =delete") == cwcc::method{false, "func", {}, false, cwcc::ref_t::lvalue, false, "int", true});
	REQUIRE(method("auto func() const &          -> int =delete") == cwcc::method{false, "func", {}, true, cwcc::ref_t::lvalue, false, "int", true});
	REQUIRE(method("auto func() const & noexcept -> int =delete") == cwcc::method{false, "func", {}, true, cwcc::ref_t::lvalue, true, "int", true});
	REQUIRE(method("auto func()       & noexcept -> int =delete") == cwcc::method{false, "func", {}, false, cwcc::ref_t::lvalue, true, "int", true});

	REQUIRE(method("auto func()       &&          -> int =delete") == cwcc::method{false, "func", {}, false, cwcc::ref_t::rvalue, false, "int", true});
	REQUIRE(method("auto func() const &&          -> int =delete") == cwcc::method{false, "func", {}, true, cwcc::ref_t::rvalue, false, "int", true});
	REQUIRE(method("auto func() const && noexcept -> int =delete") == cwcc::method{false, "func", {}, true, cwcc::ref_t::rvalue, true, "int", true});
	REQUIRE(method("auto func()       && noexcept -> int =delete") == cwcc::method{false, "func", {}, false, cwcc::ref_t::rvalue, true, "int", true});
}

TEST_CASE("parsing_method_operator", "[parsing] [method]") {
	auto method{[](const char * str) {
		std::stringstream ss{str};
		cwcc::parser p{ss};
		return cwcc::method{p, {}, {}};
	}};

	REQUIRE(method("void operator()()") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::none, false, {}, false});
	REQUIRE(method("void operator()() const") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::none, false, {}, false});
	REQUIRE(method("void operator()() const noexcept") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::none, true, {}, false});
	REQUIRE(method("void operator()()       noexcept") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::none, true, {}, false});

	REQUIRE(method("void operator()()       &") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::lvalue, false, {}, false});
	REQUIRE(method("void operator()() const &") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::lvalue, false, {}, false});
	REQUIRE(method("void operator()() const & noexcept") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::lvalue, true, {}, false});
	REQUIRE(method("void operator()()       & noexcept") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::lvalue, true, {}, false});

	REQUIRE(method("void operator()()       &&") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::rvalue, false, {}, false});
	REQUIRE(method("void operator()() const &&") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::rvalue, false, {}, false});
	REQUIRE(method("void operator()() const && noexcept") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::rvalue, true, {}, false});
	REQUIRE(method("void operator()()       && noexcept") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::rvalue, true, {}, false});


	REQUIRE(method("auto operator()()                -> int") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::none, false, "int", false});
	REQUIRE(method("auto operator()() const          -> int") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::none, false, "int", false});
	REQUIRE(method("auto operator()() const noexcept -> int") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::none, true, "int", false});
	REQUIRE(method("auto operator()()       noexcept -> int") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::none, true, "int", false});

	REQUIRE(method("auto operator()()       &          -> int") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::lvalue, false, "int", false});
	REQUIRE(method("auto operator()() const &          -> int") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::lvalue, false, "int", false});
	REQUIRE(method("auto operator()() const & noexcept -> int") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::lvalue, true, "int", false});
	REQUIRE(method("auto operator()()       & noexcept -> int") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::lvalue, true, "int", false});

	REQUIRE(method("auto operator()()       &&          -> int") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::rvalue, false, "int", false});
	REQUIRE(method("auto operator()() const &&          -> int") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::rvalue, false, "int", false});
	REQUIRE(method("auto operator()() const && noexcept -> int") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::rvalue, true, "int", false});
	REQUIRE(method("auto operator()()       && noexcept -> int") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::rvalue, true, "int", false});
	 
	REQUIRE(method("void operator()()                =delete") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::none, false, {}, true});
	REQUIRE(method("void operator()() const          =delete") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::none, false, {}, true});
	REQUIRE(method("void operator()() const noexcept =delete") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::none, true, {}, true});
	REQUIRE(method("void operator()()       noexcept =delete") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::none, true, {}, true});

	REQUIRE(method("void operator()()       &          =delete") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::lvalue, false, {}, true});
	REQUIRE(method("void operator()() const &          =delete") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::lvalue, false, {}, true});
	REQUIRE(method("void operator()() const & noexcept =delete") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::lvalue, true, {}, true});
	REQUIRE(method("void operator()()       & noexcept =delete") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::lvalue, true, {}, true});

	REQUIRE(method("void operator()()       &&          =delete") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::rvalue, false, {}, true});
	REQUIRE(method("void operator()() const &&          =delete") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::rvalue, false, {}, true});
	REQUIRE(method("void operator()() const && noexcept =delete") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::rvalue, true, {}, true});
	REQUIRE(method("void operator()()       && noexcept =delete") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::rvalue, true, {}, true});


	REQUIRE(method("auto operator()()                -> int =delete") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::none, false, "int", true});
	REQUIRE(method("auto operator()() const          -> int =delete") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::none, false, "int", true});
	REQUIRE(method("auto operator()() const noexcept -> int =delete") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::none, true, "int", true});
	REQUIRE(method("auto operator()()       noexcept -> int =delete") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::none, true, "int", true});

	REQUIRE(method("auto operator()()       &          -> int =delete") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::lvalue, false, "int", true});
	REQUIRE(method("auto operator()() const &          -> int =delete") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::lvalue, false, "int", true});
	REQUIRE(method("auto operator()() const & noexcept -> int =delete") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::lvalue, true, "int", true});
	REQUIRE(method("auto operator()()       & noexcept -> int =delete") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::lvalue, true, "int", true});

	REQUIRE(method("auto operator()()       &&          -> int =delete") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::rvalue, false, "int", true});
	REQUIRE(method("auto operator()() const &&          -> int =delete") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::rvalue, false, "int", true});
	REQUIRE(method("auto operator()() const && noexcept -> int =delete") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::rvalue, true, "int", true});
	REQUIRE(method("auto operator()()       && noexcept -> int =delete") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::rvalue, true, "int", true});
}

TEST_CASE("parsing_static method", "[parsing] [method]") {
	auto method{[](const char * str) {
		std::stringstream ss{str};
		cwcc::parser p{ss};
		return cwcc::method{p, {}, {}};
	}};

	REQUIRE(method("static void func()") == cwcc::method{true, "func", {}, false, cwcc::ref_t::none, false, {}, false});
	REQUIRE(method("static void func() noexcept") == cwcc::method{true, "func", {}, false, cwcc::ref_t::none, true, {}, false});
	REQUIRE(method("static void func()          =delete") == cwcc::method{true, "func", {}, false, cwcc::ref_t::none, false, {}, true});
	REQUIRE(method("static void func() noexcept =delete") == cwcc::method{true, "func", {}, false, cwcc::ref_t::none, true, {}, true});
	REQUIRE(method("static auto func()          -> int") == cwcc::method{true, "func", {}, false, cwcc::ref_t::none, false, "int", false});
	REQUIRE(method("static auto func() noexcept -> int") == cwcc::method{true, "func", {}, false, cwcc::ref_t::none, true, "int", false});
	REQUIRE(method("static auto func()          -> int =delete") == cwcc::method{true, "func", {}, false, cwcc::ref_t::none, false, "int", true});
	REQUIRE(method("static auto func() noexcept -> int =delete") == cwcc::method{true, "func", {}, false, cwcc::ref_t::none, true, "int", true});
}

TEST_CASE("parsing_component", "[parsing] [component]") {
	auto component{[](const char * str) {
		std::stringstream ss{str};
		cwcc::parser p{ss};
		return cwcc::component{p};
	}};

	REQUIRE(component("@library(\"dll\") component comp {};") == cwcc::component{"\"dll\"", "comp", false, {{{}, false}} , {}});
	REQUIRE(component("@library(\"dll\") component comp final {};") == cwcc::component{"\"dll\"", "comp", true, {{{}, false}}, {}});

	REQUIRE(component("@library(\"dll\") component comp { comp(int val); };") == cwcc::component{"\"dll\"", "comp", false, {{cwcc::param_list{{{false, "int", cwcc::ref_t::none, "val", ""}}}, false}} , {}});
	REQUIRE(component("@library(\"dll\") component comp final { comp(int val); };") == cwcc::component{"\"dll\"", "comp", true, {{cwcc::param_list{{{false, "int", cwcc::ref_t::none, "val", ""}}}, false}}, {}});
}

TEST_CASE("parsing_namespace", "[parsing] [namespace]") {
	auto namespace_{[](const char * str) {
		std::stringstream ss{str};
		cwcc::parser p{ss};
		return cwcc::namespace_{p};
	}};

	REQUIRE(namespace_("namespace cwc {}") == cwcc::namespace_{"cwc", {}});
	REQUIRE(namespace_("namespace cwc::xyz {}") == cwcc::namespace_{"cwc::xyz", {}});

	REQUIRE_THROWS(namespace_("namespace {}"));
	REQUIRE_THROWS(namespace_("namespace cwc: {}"));
	REQUIRE_THROWS(namespace_("namespace cwc:xyz {}"));
}
