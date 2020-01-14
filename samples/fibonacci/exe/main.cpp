
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iomanip>
#include <iostream>
#include "../cwc.sample.fibonacci.cwch"

struct fibonacci_generator final {
	using interface = cwc::sample::fibonacci::sequence;
	using component = cwc::sample::fibonacci::generator;

	static
	constexpr
	std::string_view dll{"sample-fibonacci"};
};

#include "cwc/host.hpp"

int main() {
	cwc::sized_error_handle<64> error;
	cwc::context context;
	const auto generator{context.create<fibonacci_generator>(error)};


	try {
		for(cwc::uint8 i{0}; i < 99; ++i) std::cout << "fibonacci(" << std::setw(2) << static_cast<int>(i) << ") = " << std::right << std::setw(20) << generator->calculate(error, i) << '\n';
	} catch(const std::exception & exc) {
		std::cerr << typeid(exc).name() << ": \"" << exc.what() << "\"\n";
	}

	error.clear();

	std::cout << "\n\n\n";

	generator->calculate(error, 5, 10, [&](auto value) { std::cout << value << "\n"; });
}
