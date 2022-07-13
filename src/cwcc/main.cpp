
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <fstream>
#include <sstream>
#include <iostream>
#include "ast.hpp"
#include "parser.hpp"
#include "indent.hpp"
#include "generator.hpp"

int main(int argc, char * argv[]) try {
	if(argc == 1) {
		std::cout << "usage: " << argv[0] << " <input> <output>\n";
		return EXIT_SUCCESS;
	}
	if(argc != 3) throw std::invalid_argument{"invalid count of parameters"};

	std::ifstream is{argv[1]};
	if(!is) throw std::runtime_error{"could not open input file"};
	std::ofstream os{argv[2], std::ios::binary};
	if(!os) throw std::runtime_error{"could not open output file"};

	const std::string cwc(std::istreambuf_iterator<char>{is}, std::istreambuf_iterator<char>{});
	cwcc::parser p{cwc};
	std::stringstream ss;
	cwcc::cwc c;
	c.parse(p);
	cwcc::generate(ss, c);
	cwcc::indent(ss, os);
} catch(const std::exception & exc) {
	std::cerr << "ERROR: " << exc.what() << std::endl;
}
