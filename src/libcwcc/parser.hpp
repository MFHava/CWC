
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <queue>
#include <iosfwd>
#include <string>

namespace cwcc {
	enum class type { name, nested, comment, type, string, system_header, };

	class parser final {
		std::queue<std::string> tokens;

		auto next() -> std::string;
	public:
		parser(std::istream & in);

		void expect(std::string_view str);

		auto accept(std::string_view str) const -> bool;
		auto consume(std::string_view str) -> bool;

		auto next(type type) -> std::string;

		auto starts_with(char ch) const -> bool;

		auto has_tokens() const noexcept -> bool { return !tokens.empty(); }
	};
}
