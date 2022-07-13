
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <catch.hpp>
#include "ast.hpp"
#include "parser.hpp"

TEST_CASE("parsing_attribute", "[parsing] [attribute]") {
	auto attribute{[](const char * str) {
		cwcc::attribute res;
		cwcc::parser p{str};
		res.parse(p);
		return res;
	}};

	REQUIRE(attribute("[[deprecated]]") == cwcc::attribute{"deprecated"});
	REQUIRE(attribute("[[nodiscard(\"\")]]") == cwcc::attribute{"nodiscard", "\"\""});

	REQUIRE(attribute("[[nodiscard(\"allocates memory\")]]") == cwcc::attribute{"nodiscard", "\"allocates memory\""});
	REQUIRE(attribute("[[deprecated(\"outdated interface\")]]") == cwcc::attribute{"deprecated", "\"outdated interface\""});

	REQUIRE_THROWS(attribute("[[nodiscard()"));
	REQUIRE_THROWS(attribute("[[nodiscard, deprecated]]"));
}

TEST_CASE("parsing_param", "[parsing] [param]") {
	auto param{[](const char * str) {
		cwcc::param res;
		cwcc::parser p{str};
		res.parse(p);
		return res;
	}};

	REQUIRE(param("int val") == cwcc::param{false, "int", cwcc::ref_t::none, "val"});
	REQUIRE(param("int & val") == cwcc::param{false, "int", cwcc::ref_t::lvalue, "val"});
	REQUIRE(param("int && val") == cwcc::param{false, "int", cwcc::ref_t::rvalue, "val"});
	REQUIRE(param("const int & val") == cwcc::param{true, "int", cwcc::ref_t::lvalue, "val"});

	REQUIRE_THROWS(param("const int val"));
	REQUIRE_THROWS(param("const int val //!< test test"));
}

TEST_CASE("parsing_comment", "[parsing] [comment]") {
	auto comment{[](const char * str) {
		cwcc::comment res;
		cwcc::parser p{str};
		res.parse(p);
		return res;
	}};

	REQUIRE(comment("//test") == cwcc::comment{"//test"});
	REQUIRE(comment("//test\n") == cwcc::comment{"//test\n"});

	REQUIRE_THROWS(comment(""));
	REQUIRE_THROWS(comment("*"));
	REQUIRE_THROWS(comment("/*"));
	REQUIRE_THROWS(comment("/**/"));
}

TEST_CASE("parsing_constructors", "[parsing] [constructors]") {
	auto constructor{[](const char * str) {
		cwcc::constructor res;
		cwcc::parser p{str};
		res.parse(p);
		return res;
	}};

	REQUIRE(constructor("X();") == cwcc::constructor{"X", {}, false});
	REQUIRE(constructor("X(int val);") == cwcc::constructor{"X", {{false, "int", cwcc::ref_t::none, "val"}}, false});
	REQUIRE(constructor("X() =delete;") == cwcc::constructor{"X", {}, true});
	REQUIRE(constructor("X(int val) =delete;") == cwcc::constructor{"X", {{false, "int", cwcc::ref_t::none, "val"}}, true});

	REQUIRE_THROWS(constructor("X() =default;"));
	REQUIRE_THROWS(constructor("X(int val) =default;"));
}

TEST_CASE("parsing_method", "[parsing] [method]") {
	auto method{[](const char * str) {
		cwcc::method res;
		cwcc::parser p{str};
		res.parse(p);
		return res;
	}};

	REQUIRE(method("void func();") == cwcc::method{false, "func", {}, false, cwcc::ref_t::none, false, {}, false});
	REQUIRE(method("void func() const;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::none, false, {}, false});
	REQUIRE(method("void func() const noexcept;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::none, true, {}, false});
	REQUIRE(method("void func()       noexcept;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::none, true, {}, false});

	REQUIRE(method("void func()       &;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::lvalue, false, {}, false});
	REQUIRE(method("void func() const &;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::lvalue, false, {}, false});
	REQUIRE(method("void func() const & noexcept;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::lvalue, true, {}, false});
	REQUIRE(method("void func()       & noexcept;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::lvalue, true, {}, false});

	REQUIRE(method("void func()       &&;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::rvalue, false, {}, false});
	REQUIRE(method("void func() const &&;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::rvalue, false, {}, false});
	REQUIRE(method("void func() const && noexcept;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::rvalue, true, {}, false});
	REQUIRE(method("void func()       && noexcept;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::rvalue, true, {}, false});


	REQUIRE(method("auto func()                -> int;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::none, false, "int", false});
	REQUIRE(method("auto func() const          -> int;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::none, false, "int", false});
	REQUIRE(method("auto func() const noexcept -> int;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::none, true, "int", false});
	REQUIRE(method("auto func()       noexcept -> int;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::none, true, "int", false});

	REQUIRE(method("auto func()       &          -> int;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::lvalue, false, "int", false});
	REQUIRE(method("auto func() const &          -> int;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::lvalue, false, "int", false});
	REQUIRE(method("auto func() const & noexcept -> int;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::lvalue, true, "int", false});
	REQUIRE(method("auto func()       & noexcept -> int;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::lvalue, true, "int", false});

	REQUIRE(method("auto func()       &&          -> int;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::rvalue, false, "int", false});
	REQUIRE(method("auto func() const &&          -> int;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::rvalue, false, "int", false});
	REQUIRE(method("auto func() const && noexcept -> int;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::rvalue, true, "int", false});
	REQUIRE(method("auto func()       && noexcept -> int;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::rvalue, true, "int", false});
	 
	REQUIRE(method("void func()                =delete;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::none, false, {}, true});
	REQUIRE(method("void func() const          =delete;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::none, false, {}, true});
	REQUIRE(method("void func() const noexcept =delete;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::none, true, {}, true});
	REQUIRE(method("void func()       noexcept =delete;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::none, true, {}, true});

	REQUIRE(method("void func()       &          =delete;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::lvalue, false, {}, true});
	REQUIRE(method("void func() const &          =delete;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::lvalue, false, {}, true});
	REQUIRE(method("void func() const & noexcept =delete;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::lvalue, true, {}, true});
	REQUIRE(method("void func()       & noexcept =delete;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::lvalue, true, {}, true});

	REQUIRE(method("void func()       &&          =delete;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::rvalue, false, {}, true});
	REQUIRE(method("void func() const &&          =delete;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::rvalue, false, {}, true});
	REQUIRE(method("void func() const && noexcept =delete;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::rvalue, true, {}, true});
	REQUIRE(method("void func()       && noexcept =delete;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::rvalue, true, {}, true});


	REQUIRE(method("auto func()                -> int =delete;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::none, false, "int", true});
	REQUIRE(method("auto func() const          -> int =delete;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::none, false, "int", true});
	REQUIRE(method("auto func() const noexcept -> int =delete;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::none, true, "int", true});
	REQUIRE(method("auto func()       noexcept -> int =delete;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::none, true, "int", true});

	REQUIRE(method("auto func()       &          -> int =delete;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::lvalue, false, "int", true});
	REQUIRE(method("auto func() const &          -> int =delete;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::lvalue, false, "int", true});
	REQUIRE(method("auto func() const & noexcept -> int =delete;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::lvalue, true, "int", true});
	REQUIRE(method("auto func()       & noexcept -> int =delete;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::lvalue, true, "int", true});

	REQUIRE(method("auto func()       &&          -> int =delete;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::rvalue, false, "int", true});
	REQUIRE(method("auto func() const &&          -> int =delete;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::rvalue, false, "int", true});
	REQUIRE(method("auto func() const && noexcept -> int =delete;") == cwcc::method{false, "func", {}, true, cwcc::ref_t::rvalue, true, "int", true});
	REQUIRE(method("auto func()       && noexcept -> int =delete;") == cwcc::method{false, "func", {}, false, cwcc::ref_t::rvalue, true, "int", true});
}

TEST_CASE("parsing_method_operator", "[parsing] [method]") {
	auto method{[](const char * str) {
		cwcc::method res;
		cwcc::parser p{str};
		res.parse(p);
		return res;
	}};

	REQUIRE(method("void operator()();") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::none, false, {}, false});
	REQUIRE(method("void operator()() const;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::none, false, {}, false});
	REQUIRE(method("void operator()() const noexcept;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::none, true, {}, false});
	REQUIRE(method("void operator()()       noexcept;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::none, true, {}, false});

	REQUIRE(method("void operator()()       &;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::lvalue, false, {}, false});
	REQUIRE(method("void operator()() const &;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::lvalue, false, {}, false});
	REQUIRE(method("void operator()() const & noexcept;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::lvalue, true, {}, false});
	REQUIRE(method("void operator()()       & noexcept;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::lvalue, true, {}, false});

	REQUIRE(method("void operator()()       &&;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::rvalue, false, {}, false});
	REQUIRE(method("void operator()() const &&;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::rvalue, false, {}, false});
	REQUIRE(method("void operator()() const && noexcept;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::rvalue, true, {}, false});
	REQUIRE(method("void operator()()       && noexcept;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::rvalue, true, {}, false});


	REQUIRE(method("auto operator()()                -> int;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::none, false, "int", false});
	REQUIRE(method("auto operator()() const          -> int;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::none, false, "int", false});
	REQUIRE(method("auto operator()() const noexcept -> int;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::none, true, "int", false});
	REQUIRE(method("auto operator()()       noexcept -> int;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::none, true, "int", false});

	REQUIRE(method("auto operator()()       &          -> int;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::lvalue, false, "int", false});
	REQUIRE(method("auto operator()() const &          -> int;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::lvalue, false, "int", false});
	REQUIRE(method("auto operator()() const & noexcept -> int;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::lvalue, true, "int", false});
	REQUIRE(method("auto operator()()       & noexcept -> int;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::lvalue, true, "int", false});

	REQUIRE(method("auto operator()()       &&          -> int;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::rvalue, false, "int", false});
	REQUIRE(method("auto operator()() const &&          -> int;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::rvalue, false, "int", false});
	REQUIRE(method("auto operator()() const && noexcept -> int;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::rvalue, true, "int", false});
	REQUIRE(method("auto operator()()       && noexcept -> int;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::rvalue, true, "int", false});
	 
	REQUIRE(method("void operator()()                =delete;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::none, false, {}, true});
	REQUIRE(method("void operator()() const          =delete;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::none, false, {}, true});
	REQUIRE(method("void operator()() const noexcept =delete;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::none, true, {}, true});
	REQUIRE(method("void operator()()       noexcept =delete;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::none, true, {}, true});

	REQUIRE(method("void operator()()       &          =delete;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::lvalue, false, {}, true});
	REQUIRE(method("void operator()() const &          =delete;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::lvalue, false, {}, true});
	REQUIRE(method("void operator()() const & noexcept =delete;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::lvalue, true, {}, true});
	REQUIRE(method("void operator()()       & noexcept =delete;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::lvalue, true, {}, true});

	REQUIRE(method("void operator()()       &&          =delete;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::rvalue, false, {}, true});
	REQUIRE(method("void operator()() const &&          =delete;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::rvalue, false, {}, true});
	REQUIRE(method("void operator()() const && noexcept =delete;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::rvalue, true, {}, true});
	REQUIRE(method("void operator()()       && noexcept =delete;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::rvalue, true, {}, true});


	REQUIRE(method("auto operator()()                -> int =delete;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::none, false, "int", true});
	REQUIRE(method("auto operator()() const          -> int =delete;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::none, false, "int", true});
	REQUIRE(method("auto operator()() const noexcept -> int =delete;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::none, true, "int", true});
	REQUIRE(method("auto operator()()       noexcept -> int =delete;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::none, true, "int", true});

	REQUIRE(method("auto operator()()       &          -> int =delete;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::lvalue, false, "int", true});
	REQUIRE(method("auto operator()() const &          -> int =delete;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::lvalue, false, "int", true});
	REQUIRE(method("auto operator()() const & noexcept -> int =delete;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::lvalue, true, "int", true});
	REQUIRE(method("auto operator()()       & noexcept -> int =delete;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::lvalue, true, "int", true});

	REQUIRE(method("auto operator()()       &&          -> int =delete;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::rvalue, false, "int", true});
	REQUIRE(method("auto operator()() const &&          -> int =delete;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::rvalue, false, "int", true});
	REQUIRE(method("auto operator()() const && noexcept -> int =delete;") == cwcc::method{false, "operator()", {}, true, cwcc::ref_t::rvalue, true, "int", true});
	REQUIRE(method("auto operator()()       && noexcept -> int =delete;") == cwcc::method{false, "operator()", {}, false, cwcc::ref_t::rvalue, true, "int", true});
}

TEST_CASE("parsing_static method", "[parsing] [method]") {
	auto method{[](const char * str) {
		cwcc::method res;
		cwcc::parser p{str};
		res.parse(p);
		return res;
	}};

	REQUIRE(method("static void func();") == cwcc::method{true, "func", {}, false, cwcc::ref_t::none, false, {}, false});
	REQUIRE(method("static void func() noexcept;") == cwcc::method{true, "func", {}, false, cwcc::ref_t::none, true, {}, false});
	REQUIRE(method("static void func()          =delete;") == cwcc::method{true, "func", {}, false, cwcc::ref_t::none, false, {}, true});
	REQUIRE(method("static void func() noexcept =delete;") == cwcc::method{true, "func", {}, false, cwcc::ref_t::none, true, {}, true});
	REQUIRE(method("static auto func()          -> int;") == cwcc::method{true, "func", {}, false, cwcc::ref_t::none, false, "int", false});
	REQUIRE(method("static auto func() noexcept -> int;") == cwcc::method{true, "func", {}, false, cwcc::ref_t::none, true, "int", false});
	REQUIRE(method("static auto func()          -> int =delete;") == cwcc::method{true, "func", {}, false, cwcc::ref_t::none, false, "int", true});
	REQUIRE(method("static auto func() noexcept -> int =delete;") == cwcc::method{true, "func", {}, false, cwcc::ref_t::none, true, "int", true});

	REQUIRE_THROWS(method("static void operator()();"));
}

TEST_CASE("parsing_component", "[parsing] [component]") {
	auto component{[](const char * str) {
		cwcc::component res;
		cwcc::parser p{str};
		res.parse(p);
		return res;
	}};

	REQUIRE(component("component comp {};") == cwcc::component{{}, "comp", false});
	REQUIRE(component("component comp final {};") == cwcc::component{{}, "comp", true});

	REQUIRE(component("component comp { comp(int val); };") == cwcc::component{{}, "comp", false, {cwcc::constructor{"comp", {{false, "int", cwcc::ref_t::none, "val"}}, false}}});
	REQUIRE(component("component comp final { comp(int val); };") == cwcc::component{{}, "comp", true, {cwcc::constructor{"comp", {{false, "int", cwcc::ref_t::none, "val"}}, false}}});
}

TEST_CASE("parsing_template", "[parsing] [template]") {
	auto template_{[](const char * str) {
		cwcc::template_ res;
		cwcc::parser p{str};
		res.parse(p);
		return res;
	}};

	REQUIRE(template_("template<typename T> component comp {};") == cwcc::template_{{"T"}, {{}, "comp", false}});
	REQUIRE(template_("template<typename T1, typename T2> component comp {};") == cwcc::template_{{"T1", "T2"}, {{}, "comp", false}});
	REQUIRE(template_("template<typename T_1, typename T_2> component comp {};") == cwcc::template_{{"T_1", "T_2"}, {{}, "comp", false}});

	REQUIRE_THROWS(template_("template<> component comp {};"));
}

TEST_CASE("parsing_extern", "[parsing] [extern]") {
	auto extern_{[](const char * str) {
		cwcc::extern_ res;
		cwcc::parser p{str};
		res.parse(p);
		return res;
	}};

	REQUIRE(extern_("extern template component X<int>;") == cwcc::extern_{"X", {"int"}});
	REQUIRE(extern_("extern template component X<int, float>;") == cwcc::extern_{"X", {"int", "float"}});
	REQUIRE_THROWS(extern_("extern template component X<int, float, 3>;"));
}

TEST_CASE("parsing_library", "[parsing] [library]") {
	auto library{[](const char * str) {
		cwcc::library res;
		cwcc::parser p{str};
		res.parse(p);
		return res;
	}};

	REQUIRE(library("@library(\"test\") extern template component X<int>;") == cwcc::library{"\"test\"", cwcc::extern_{"X", {"int"}}});
	REQUIRE(library("@library(\"test\") component X {};") == cwcc::library{"\"test\"", cwcc::component{{}, "X", false}});

	REQUIRE_THROWS(library("@library() component X {};"));
	REQUIRE_THROWS(library("@ library() component X {};"));
}

TEST_CASE("parsing_namespace", "[parsing] [namespace]") {
	auto namespace_{[](const char * str) {
		cwcc::namespace_ res;
		cwcc::parser p{str};
		res.parse(p);
		return res;
	}};

	REQUIRE(namespace_("namespace cwc {}") == cwcc::namespace_{"cwc"});
	REQUIRE(namespace_("namespace cwc::xyz {}") == cwcc::namespace_{"cwc::xyz"});

	REQUIRE_THROWS(namespace_("namespace {}"));
	REQUIRE_THROWS(namespace_("namespace 3 {}"));
	REQUIRE_THROWS(namespace_("namespace : {}"));
	REQUIRE_THROWS(namespace_("namespace :: {}"));
	REQUIRE_THROWS(namespace_("namespace cwc: {}"));
	REQUIRE_THROWS(namespace_("namespace cwc:xyz {}"));
}

TEST_CASE("parsing_include", "[parsing] [include]") {
	auto include{[](const char * str) {
		cwcc::include res;
		cwcc::parser p{str};
		res.parse(p);
		return res;
	}};

	REQUIRE(include("#include \"test\"") == cwcc::include{"\"test\""});
	REQUIRE(include("#include\"test\"") == cwcc::include{"\"test\""});
	REQUIRE(include("#include <test>") == cwcc::include{"<test>"});
	REQUIRE(include("#include<test>") == cwcc::include{"<test>"});

	REQUIRE_THROWS(include("in"));
	REQUIRE_THROWS(include("#in"));
	REQUIRE_THROWS(include("#include"));
	REQUIRE_THROWS(include("#include <"));
	REQUIRE_THROWS(include("#include >"));
	REQUIRE_THROWS(include("#include \""));
	REQUIRE_THROWS(include("#include <\""));
	REQUIRE_THROWS(include("#include \">"));
}

TEST_CASE("parsing_cwc", "[parsing] [cwc]") {
	auto cwc{[](const char * str) {
		cwcc::cwc res;
		cwcc::parser p{str};
		res.parse(p);
		return res;
	}};

	REQUIRE(cwc("#include <iostream>") == cwcc::cwc{{cwcc::include{"<iostream>"}}});
	REQUIRE(cwc("#include <iostream>\n//test") == cwcc::cwc{{cwcc::include{"<iostream>"}, cwcc::comment{"//test"}}});
	REQUIRE(cwc("namespace test {}") == cwcc::cwc{{cwcc::namespace_{"test"}}});
	REQUIRE(cwc("namespace test {} namespace xyz {}") == cwcc::cwc{{cwcc::namespace_{"test"}, cwcc::namespace_{"xyz"}}});

	REQUIRE_THROWS(cwc("namespace test { namespace xyz {} }"));
}
