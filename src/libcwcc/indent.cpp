
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <istream>
#include <ostream>
#include <iterator>
#include "indent.hpp"

namespace cwcc {
	void indent(std::istream & is, std::ostream & os) {
		std::size_t indent{0};

		for(std::string line; std::getline(is, line);) {
			if(line.empty()) {
				os << "\n";
				continue;
			}

			if(line.front() == '#');
			else if(line.front() == ')' || line.front() == '}' || line == "private:") std::fill_n(std::ostream_iterator<char>{os}, indent - 1, '\t');
			else std::fill_n(std::ostream_iterator<char>{os}, indent, '\t');
			os << line << '\n';

			for(const char & c : line) {
				switch(c) {
					case '(':
					case '{':
						++indent;
						break;
					case ')':
					case '}':
						--indent;
						break;
					case '/': goto done;
				}
			}
	done:
			(void)0; //TODO: [C++23] will make this redundant
		}
	}
}
