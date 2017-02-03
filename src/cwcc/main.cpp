
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <fstream>
#include <sstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include "nodes.hpp"
#include "utils.hpp"
#include "parser.hpp"
#include "library.hpp"
#include "disclaimer.hpp"
#include "generators.hpp"

namespace {
	enum class type {
		extracted,
		generated,
		stub
	};

	auto operator<<(std::ostream & os, const type & t) -> std::ostream & {
		switch(t) {
			case type::extracted: return os << "extracted";
			case type::generated: return os << "generated";
			case type::stub:      return os << "stub";
			default: std::terminate();
		}
	}

	auto allow_write(const std::string & filename, type type) -> bool {
		if(boost::filesystem::exists(filename)) {
			for(;;) {
				std::cout << "CWCC is trying to override the existing file '" << filename << "' with " << type << "-file. Proceed? (Y/y/N/n): " << std::flush;
				std::string line;
				std::getline(std::cin, line);
				if(line.size() == 1)
					switch(line[0]) {
						case 'y':
						case 'Y':
							return true;
						case 'n':
						case 'N':
							return false;
					}
			}
		}
		return true;
	}

	template<typename WriteFunctor>
	void conditional_write(const std::string & filename, type type, WriteFunctor functor) {
		if(allow_write(filename, type)) {
			std::ofstream os{filename};
			os.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
			functor(os);
			os.flush();
		}
	}

	void generate_files(const cwcc::bundle & bundle, std::istream & definition) {
		const auto base_name = cwcc::base_file_name(bundle.name) + '.';
		conditional_write(base_name + "cwch", type::generated, [&](std::ostream & os) { cwcc::generate_header(os, bundle); });
		conditional_write(base_name + "cwc",  type::generated, [&](std::ostream & os) { cwcc::generate_source(definition, os, bundle); });
		conditional_write(base_name + "h",    type::stub,      [&](std::ostream & os) { cwcc::generate_stub(os, bundle); });
	}

	void reflect(const char * filename) {
		cwcc::library lib{filename};
		std::istringstream in{lib.definition()};
		const auto bundle = cwcc::parse(in);
		in.seekg(0).clear();
		const auto base_name = cwcc::base_file_name(bundle.name) + '.';

		conditional_write(base_name + "bdl",  type::extracted, [&](std::ostream & os) { os << lib.definition(); });
		generate_files(bundle, in);
	}

	void compile(const char * filename) {
		std::ifstream in{filename};
		const auto bundle = cwcc::parse(in);
		in.seekg(0).clear();
		const auto base_name = cwcc::base_file_name(bundle.name) + '.';

		generate_files(bundle, in);
	}
}

int main(int argc, char * argv[]) {
	cwcc::program_disclaimer(std::cout);
	if(argc != 2) {//batch operations are NOT supported!
		std::cout << "\nusage:\n"
		             "\tCWCC bundle => .cwch (generated), .cwc (generated), .h (stub), .bdl (extracted)\n"
		             "\tCWCC bdl    => .cwch (generated), .cwc (generated), .h (stub)"
		          << std::endl;
	} else try {
		const boost::filesystem::path path{argv[1]};
		if(path.extension() != ".bdl") reflect(argv[1]);
		else compile(argv[1]);
	} catch(const std::exception & exc) {
		std::cerr << "error occured while processing \"" << argv[1] << "\":\n" << exc.what() << std::endl;
	}
}