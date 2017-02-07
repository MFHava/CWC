
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <ostream>
#include "nodes.hpp"

namespace cwcc {
	template<typename Visitor>
	void wrap_in_namespace(std::ostream & os, const bundle & b, bool comments, const char * delimiter = "") {
		//TODO: this will be simplified once nested namespaces (C++17) can be used
		auto namespaces = 0;
		{
			for(const auto & doc : b.lines) os << doc << '\n';
			auto it = std::begin(b.name);
			for(;;) {
				const auto tmp = std::find(it, std::end(b.name), ':');
				std::copy(it, tmp, std::ostream_iterator<char>(os << "namespace "));
				os << " {";
				++namespaces;
				if(tmp == std::end(b.name)) break;
				it = tmp + 2;
			}
			os << '\n';
		}
		{
			Visitor visitor{os, b};
			auto it = std::begin(b.members);
			const auto end = std::end(b.members);
			if(it != end) {
				it->apply_visitor(visitor);
				for(++it; it != end; ++it) {
					os << delimiter;
					it->apply_visitor(visitor);
				}
			}
		}
		std::fill_n(std::ostream_iterator<char>{os, " "}, namespaces, '}');
	}
}