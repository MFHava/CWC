
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <stdexcept>
#include "../cwc.sample.fibonacci.cwch"

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

CWC_EXPORT_cwc$sample$fibonacci$sequence(fibonacci_sequence);
