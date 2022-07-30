
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "fibonacci.cwch"

#ifndef CWC_SAMPLE_DLL
#include <iomanip>
#include <iostream>

int main() try {
	std::cout << "sequence is available: " << std::boolalpha << cwc::available<cwc::sample::fibonacci::sequence<std::uint8_t, 1>>() << std::noboolalpha << "\n";

	cwc::sample::fibonacci::sequence<std::uint8_t, 1> s;

	std::cout << "\n\n\n";

	for(std::uint8_t i{0}; i < 94; ++i) std::cout << "fibonacci(" << std::setw(2) << static_cast<int>(i) << ") = " << std::right << std::setw(20) << s.calculate(i) << '\n';

	std::cout << "\n\n\n";

	try {
		const auto max{s.max()};
		std::cout << "MAX: " << static_cast<int>(max) << '\n';
		(void)s.calculate(max + 1);
	} catch(const std::exception & exc) {
		std::cout << "ERROR(" << typeid(exc).name() << "): " << exc.what() << "\n";
	}
} catch(const std::exception & exc) {
	std::cout << "FATAL ERROR: " << typeid(exc).name() << ": \"" << exc.what() << "\"\n";
}
#else
#include <stdexcept>

namespace {
	struct fibonacci_sequence {
		fibonacci_sequence() noexcept { printf("%s()\n", __func__); }

		~fibonacci_sequence() noexcept { printf("%s()\n", __func__); }

		auto calculate(std::uint8_t no) const -> std::uint64_t {
			if(no > max()) throw std::out_of_range{"fibonacci number 93 is the last to fit into uint64"};
			std::uint64_t a{0}, b{1};
			for(decltype(no) i{0}; i < no; ++i) {
				const auto c{a + b};
				a = b;
				b = c;
			}
			return a;
		}

		static
		auto max() noexcept -> std::uint8_t { return 93; }
	};
}

CWC_EXPORT_3cwc6sample9fibonacci8sequenceT3std7uint8_t_1E(fibonacci_sequence);
#endif
