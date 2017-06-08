
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <vector>
#include <iostream>
#include "../cwc.sample.fibonacci.cwch"

#define USE_PLUGIN
#ifdef USE_PLUGIN
	#define CWC_CONTEXT_INIT_STRING\
	"[cwc.mapping]\n"\
	"cwc::sample::fibonacci::generator = [generator]\n"\
	"[generator]\n"\
	"ID = sample-fibonacci-dll.dll"
#else
	#define CWC_CONTEXT_INIT_STRING\
	"[cwc.mapping]\n"\
	"cwc::sample::fibonacci::generator = sample-fibonacci-dll.dll"
#endif
#define CWC_CONTEXT_INIT_IS_NOT_FILE
#include "cwc/host.hpp"

int main() try {
	const auto context = cwc::this_context();

	std::cout << "CWC " << context->version() << std::endl;
#if 1
	for(auto section : context->config()) {
		std::cout << '[' << section.name << "]\n";
		for(const auto & entry : section.enumerator) {
			std::cout << entry.key << " = " << entry.value << std::endl;
		}
		std::cout << std::endl;
	}
#else
	for(auto t = context->config(); !t->end(); t->next()) {
		auto section = t->get();
		std::cout << '[' << section.name << "]\n";
		for(auto tt = section.enumerator; !tt->end(); tt->next()) {
			const auto & cur = tt->get();
			std::cout << cur.key << " = " << cur.value << std::endl;
		}
		std::cout << std::endl;
	}
#endif

#ifndef USE_PLUGIN
	auto factory = cwc::this_context()->factory<cwc::sample::fibonacci::generator>();
#else
	using namespace cwc::literals;
	auto factory = cwc::this_context()->factory<cwc::sample::fibonacci::generator>("ID"_sr);
#endif
	cwc::intrusive_ptr<cwc::sample::fibonacci::sequence> generator{factory->create()};

	std::cout << "calculate fibonacci no: " << std::flush;
	std::string tmp;
	std::getline(std::cin, tmp);
	const auto result = generator->calculate(std::atoi(tmp.c_str()));
	std::cout << "result: " << result << std::endl;
	
	std::cin.get(); std::cin.get();
} catch(const std::exception & exc) {
	std::cerr << "ERROR(" << typeid(exc).name() << "): " << exc.what() << std::endl;
	std::cin.get(); std::cin.get();
} catch(...) {
	std::cerr << "ERROR: <non-exception thrown>" << std::endl;
	std::cin.get(); std::cin.get();
}