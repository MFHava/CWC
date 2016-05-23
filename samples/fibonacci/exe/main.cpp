
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include "../cwc.sample.fibonacci.cwch"

int main() {
#if 1
	cwc::this_context::init(cwc::this_context::init_mode::string,
		"[cwc.mapping]\n"
		"cwc::sample::fibonacci::generator = [ids]\n"
		"[ids]\n"
		"ID = sample-fibonacci-dll.dll\n"
	);
	cwc::sample::fibonacci::generator fibonacci{cwc::plugin_id{"ID"}};
	assert(cwc::this_context::plugin_available<cwc::sample::fibonacci::generator>(cwc::plugin_id{"ID"}));
	assert(!cwc::this_context::plugin_available<cwc::sample::fibonacci::generator>(cwc::plugin_id{"XID"}));
#else

	cwc::this_context::init(cwc::this_context::init_mode::string,
		"[cwc.mapping]\n"
		"cwc::sample::fibonacci::generator = sample-fibonacci-dll.dll"
	);

	cwc::sample::fibonacci::generator fibonacci;
#endif
	for(auto t = cwc::this_context::config(); !t.end(); t.next()) {
		std::cout << t.get() << std::endl;
		for(auto tt = cwc::this_context::config(t.get()); !tt.end(); tt.next()) {
			const auto & cur = tt.get();
			std::cout << cur.key << " = " << cur.value << std::endl;
		}
		std::cout << std::endl;
	}

	std::cout << "calculate fibonacci no: " << std::flush;
	std::string tmp;
	std::getline(std::cin, tmp);
	const auto number = std::atoi(tmp.c_str());

	try {
		const auto result = fibonacci(number);
		std::cout << "result: " << result << std::endl;

	} catch(const std::exception & exc) {
		std::cerr << exc.what() << std::endl;
	}

	std::cin.get(); std::cin.get();
}