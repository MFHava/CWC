
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iomanip>
#include <iostream>
#include "../cwc.sample.fibonacci.cwch"

inline
constexpr
cwc::import<cwc::sample::fibonacci::generator> fibonacci_generator{"sample-fibonacci"};

namespace {
	auto create() -> cwc::handle<cwc::sample::fibonacci::sequence> {
		ptl::array<char, 64> error_buffer;

		cwc::loader loader;
		try {
			const auto factory{loader.factory(fibonacci_generator, cwc::error_context{error_buffer})};
			const cwc::handle<cwc::sample::fibonacci::sequence> generator{factory->create(cwc::error_context{error_buffer})};

			for(cwc::uint8 i{0}; i < 99; ++i) std::cout << "fibonacci(" << std::setw(2) << static_cast<int>(i) << ") = " << std::right << std::setw(20) << generator->calculate(i) << '\n';
		} catch(const std::exception & exc) {
			std::cerr << typeid(exc).name() << ": \"" << exc.what() << "\"\n";
		}

		return loader.create(fibonacci_generator);
	}
}

int main() {
	auto generator = create();

	std::cout << "\n\n\n";

	{
		generator->calculate(5, 10, [&](auto value) { std::cout << value << "\n"; });
	}
}
