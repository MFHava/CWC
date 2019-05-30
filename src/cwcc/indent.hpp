
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <ostream>

namespace cwcc {
	struct indent_scope final {
		indent_scope(std::ostream & os);
		indent_scope(const indent_scope &) =delete;
		indent_scope(indent_scope &&) noexcept =delete;
		auto operator=(const indent_scope &) -> indent_scope & =delete;
		auto operator=(indent_scope &&) noexcept -> indent_scope & =delete;
		~indent_scope() noexcept;
	private:
		std::ostream & os;
	};

	auto indent(std::ostream &) -> std::ostream &;
}
