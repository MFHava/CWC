
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iomanip>
#include <iostream>
#include "../cwc.sample.fibonacci.cwch"

#define CWC_CONTEXT_INIT_IS_NOT_FILE
#define CWC_CONTEXT_INIT_STRING\
	"[cwc.mapping]\n"\
	"cwc::sample::fibonacci::generator = sample-fibonacci"
#include "cwc/host.hpp"

int main() {
	cwc::sample::fibonacci::sequence_ s{cwc::intrusive_ptr<cwc::sample::fibonacci::sequence>{cwc::this_context().factory<cwc::sample::fibonacci::generator>()->create()}};

	for(cwc::uint8 i{0}; i < 94; ++i) std::cout << "fibonacci(" << std::setw(2) << static_cast<int>(i) << ") = " << std::right << std::setw(20) << s.calculate(i) << '\n';

	std::cout << "\n\n\n";

	s.calculate(5, 10, [&](auto value) { std::cout << value << "\n"; });
}
