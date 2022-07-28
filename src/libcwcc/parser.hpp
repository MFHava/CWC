
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <iosfwd>
#include <optional>
#include <string_view>

namespace cwcc {
	class parser final {
		std::string_view content; //TODO: actually keeping a pointer to end would be sufficient...
		std::string_view::const_iterator pos{content.cbegin()};

		void skip_ws() noexcept;

		auto expect_delimited(char first, char last) -> std::string_view;
	public:
		parser(std::string_view str) noexcept : content{str} { skip_ws(); }

		auto accept(std::string_view str) const noexcept -> bool;
		auto consume(std::string_view str) noexcept -> bool;
		void expect(std::string_view str);

		auto expect_namespace() -> std::string_view;
		auto expect_comment() -> std::string_view;
		auto expect_string_literal() -> std::string_view;
		auto expect_system_header() -> std::string_view;
		auto expect_name() -> std::string_view;
		auto expect_type() -> std::string_view;
		auto expect_number() -> std::string_view;
		auto expect_tparam() -> std::string_view;
		auto expect_version() -> std::string_view;

		explicit
		operator bool() const noexcept { return pos != content.cend(); }
	};
}
