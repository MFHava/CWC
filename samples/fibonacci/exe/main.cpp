
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iomanip>
#include <iostream>
#include "../cwc.sample.fibonacci.cwch"

int main() {
	cwc::sample::fibonacci::sequence_ s{1};

	std::cout << "\n\n\n";

	for(cwc::uint8 i{0}; i < 94; ++i) std::cout << "fibonacci(" << std::setw(2) << static_cast<int>(i) << ") = " << std::right << std::setw(20) << s.calculate(i) << '\n';

	std::cout << "\n\n\n";
}
