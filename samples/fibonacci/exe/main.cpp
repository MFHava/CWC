
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iomanip>
#include <iostream>
#include "../cwc.sample.fibonacci.cwch"

int main() {
	cwc::sample::fibonacci::sequence s{1};

	std::cout << "\n\n\n";

	for(std::uint8_t i{0}; i < 94; ++i) std::cout << "fibonacci(" << std::setw(2) << static_cast<int>(i) << ") = " << std::right << std::setw(20) << s.calculate(i) << '\n';

	std::cout << "\n\n\n";

	try {
		const auto max{s.max()};
		std::cout << "MAX: " << static_cast<int>(max) << '\n';
		s.calculate(max + 1);
	} catch(const std::exception & exc) {
		std::cout << "ERROR(" << typeid(exc).name() << "): " << exc.what() << "\n";
	}
}
