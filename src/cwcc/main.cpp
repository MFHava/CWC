
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <fstream>
#include <iostream>
#include <boost/program_options.hpp>
#include "nodes.hpp"
#include "utils.hpp"
#include "parser.hpp"
#include "library.hpp"
#include "disclaimer.hpp"
#include "generators.hpp"

int main(int argc, char * argv[]) try {
	namespace po = boost::program_options;
	po::options_description desc{"Allowed options"};
	desc.add_options()
		("help", "produce help message")
		("reflect,r", "extract the BDL file from a bundle")
		("exports,x", "extract exported components from a bundle")
		("header,h", "generate header for BDL")
		("source,s", "generate source for BDL")
	;

	po::options_description hidden{"Hidden options"};
	hidden.add_options()
		("input-file", po::value<std::string>()->required(), "input file")
	;

	po::options_description cmd_options{"Command line options"};
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

	if(vm.count("reflect") || vm.count("exports")) {//assume that file name is a DLL/SO
		auto & file{vm.at("input-file").as<std::string>()};
		const cwcc::library lib{file};
		if(vm.count("exports")) {
			//TODO: better layout
			std::cout << "=== exports ===\n";
			for(const auto & tmp : lib.exports()) std::cout << tmp << '\n';
			std::cout << "===============\n";
		}
		if(vm.count("reflect")) {
			file += ".bdl";//replace filename in case we want to generate header or source
			std::ofstream os{file};
			os.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
			os << lib.definition();
		}
	}

	if(vm.count("source") || vm.count("header")) {
		std::ifstream is{vm["input-file"].as<std::string>()};
		const auto bundle{cwcc::parse(is)};
		const auto base_name{cwcc::base_file_name(bundle.name) + '.'};
		if(vm.count("header")) {
			std::ofstream os{base_name + "cwch"};
			os.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
			cwcc::generate_header(os, bundle);
		}
		if(vm.count("source")) {
			is.seekg(0).clear();
			std::ofstream os{base_name + "cpp"};
			os.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
			cwcc::generate_source(is, os, bundle);
		}
	}
} catch(const std::exception & exc) {
	std::cerr << "ERROR: " << exc.what() << std::endl;
}