
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "indent.hpp"

namespace cwcc {
	namespace {
		const auto index{std::ios_base::xalloc()};
	}

	indent_scope::indent_scope(std::ostream & os) : os{os} { ++os.iword(index); }

	indent_scope::~indent_scope() noexcept { --os.iword(index); }

	auto indent(std::ostream & os) -> std::ostream & {
		const auto count{os.iword(index)};
		for(auto i{0L}; i < count; ++i) os << '\t';
		return os;
	}
}
