
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <vector>
#include <iostream>
#include "../cwc.sample.fibonacci.cwch"

int main() try {
	cwc::init(cwc::init_mode::string,
		"[cwc.mapping]\n"
		"cwc::sample::fibonacci::generator = sample-fibonacci-dll.dll"
	);
	const auto context = cwc::this_context();

	std::cout << "CWC " << context->version() << std::endl;
#if 0
	for(auto section : cwc::make_enumerator_range(context->config())) {
		std::cout << section.name << std::endl;
		for(const auto & entry : cwc::make_enumerator_range(std::move(section.enumerator))) {
			std::cout << entry.key << " = " << entry.value << std::endl;
		}
		std::cout << std::endl;
	}
#else
	for(auto t = context->config(); !t->end(); t->next()) {
		auto section = t->get();
		std::cout << section.name << std::endl;
		for(auto tt = section.enumerator; !tt->end(); tt->next()) {
			const auto & cur = tt->get();
			std::cout << cur.key << " = " << cur.value << std::endl;
		}
		std::cout << std::endl;
	}
#endif

	auto factory = cwc::this_context()->factory<cwc::sample::fibonacci::generator>();
	const auto dummy = factory->create(0);
	const auto generator = factory->create()->intrusive_from_this<cwc::sample::fibonacci::sequence>();
	//auto generator = cwc::intrusive_ptr_cast<cwc::sample::fibonacci::sequence>(factory->create());
	//cwc::intrusive_ptr<cwc::sample::fibonacci::sequence> generator{factory->create()};

	std::cout << "calculate fibonacci no: " << std::flush;
	std::string tmp;
	std::getline(std::cin, tmp);
	const auto result = generator->compute(std::atoi(tmp.c_str()));
	std::cout << "result: " << result << std::endl;
	
	std::cin.get(); std::cin.get();
} catch(const std::exception & exc) {
	std::cerr << "ERROR(" << typeid(exc).name() << "): " << exc.what() << std::endl;
	std::cin.get(); std::cin.get();
} catch(...) {
	std::cerr << "ERROR: <non-exception thrown>" << std::endl;
	std::cin.get(); std::cin.get();
}