//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <iostream>
//TODO: extensive error messages
#include "ast.hpp"
#include "parser.hpp"
#include "indent.hpp"

int main(int argc, char * argv[]) {
	//TODO: read input from argv
	std::stringstream ss{R"(
		namespace cwc::sample::fibonacci {
			@library("sample-fibonacci")
			component [[nodiscard]] sequence {
				sequence();
				[[deprecated("for visualization only")]]
				sequence(const int & dummy);

				//! @brief compute fibonacci number
				//! @returns fibonacci number
				[[nodiscard]]
				auto calculate(
					std::uint8_t no //!< [in] no
				) const -> std::uint64_t;

				//! @returns max supported fibonacci number that can be computed before result would overflow
				[[nodiscard]]
				static
				auto max() noexcept -> std::uint8_t;
			};
		}
	)"};
	try {
		cwcc::parser p{ss};
		cwcc::cwcc c{p};

		std::stringstream ss;
		c.generate(ss);
		cwcc::indent(ss, std::cout);
	} catch(const std::exception & exc) { std::cerr << "ERROR: " << exc.what() << std::endl; }

}
