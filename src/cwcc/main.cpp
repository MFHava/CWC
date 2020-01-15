
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <fstream>
#include <iostream>
#include <boost/program_options.hpp>
#include "nodes.hpp"
#include "utils.hpp"
#include "parser.hpp"
#include "disclaimer.hpp"
#include "header_generator.hpp"

int main(int argc, char * argv[]) try {
	namespace po = boost::program_options;
	po::options_description desc{"supported options"};
	desc.add_options()
		("help", "produce help message")
		("header,h", "generate header for BDL (needed to consume/implement bundle)")
	;

	po::options_description hidden{"hidden options"};
	hidden.add_options()
		("input-file", po::value<std::string>()->required(), "input file")
	;

	po::options_description cmd_options{"command line options"};
	cmd_options.add(desc).add(hidden);

	po::positional_options_description p;
	p.add("input-file", -1);

	po::variables_map vm;
	po::store(po::command_line_parser{argc, argv}.options(cmd_options).positional(p).run(), vm);

	if(vm.count("help")) {
		cwcc::program_disclaimer(std::cout);
		std::cout << desc << "\n";
		return EXIT_SUCCESS;
	}
	po::notify(vm);

	if(vm.count("header")) {
		std::ifstream is{vm["input-file"].as<std::string>()};
		const auto bundle{cwcc::parse(is)};
		const auto base_name{cwcc::base_file_name(bundle.name) + '.'};
		std::ofstream os{base_name + "cwch", std::ios::binary};
		os.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
		cwcc::generate_header(os, bundle);
	}
} catch(const std::exception & exc) {
	std::cerr << "ERROR: " << exc.what() << std::endl;
}
