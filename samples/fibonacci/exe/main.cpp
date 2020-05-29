
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

int main() {
	cwc::sized_error_context<64> error;
	
	cwc::loader loader;
	try {
		const auto factory{loader.factory<fibonacci_generator>(error)};
		const cwc::handle<cwc::sample::fibonacci::sequence> generator{factory->create(error)};

		for(cwc::uint8 i{0}; i < 99; ++i) std::cout << "fibonacci(" << std::setw(2) << static_cast<int>(i) << ") = " << std::right << std::setw(20) << generator->calculate(error, i) << '\n';
	} catch(const std::exception & exc) {
		std::cerr << typeid(exc).name() << ": \"" << exc.what() << "\"\n";
	}


	std::cout << "\n\n\n";

	{
		const auto generator{loader.create<fibonacci_generator>()};
		error.clear();
		generator->calculate(error, 5, 10, [&](auto value) { std::cout << value << "\n"; });
	}
}
