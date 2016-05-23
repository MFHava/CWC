
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <fstream>
#include <sstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include "nodes.hpp"
#include "parser.hpp"
#include "library.hpp"
#include "generators.hpp"

namespace {
	enum class type {
		extracted,
		generated,
		stub
	};

	auto operator<<(std::ostream & out, const type & t) -> std::ostream & {
		switch(t) {
			case type::extracted: return out << "extracted";
			case type::generated: return out << "generated";
			case type::stub:      return out << "stub";
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
			std::ofstream out{filename};
			out.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
			functor(out);
			out.flush();
		}
	}

	void generate_files(const cwcc::bundle & bundle, std::istream & definition) {
		const auto base_name = cwcc::base_file_name(bundle.name) + '.';
		conditional_write(base_name + "cwch", type::generated, [&](std::ostream & out) { cwcc::generate_header(out, bundle); });
		conditional_write(base_name + "cwc",  type::generated, [&](std::ostream & out) { cwcc::generate_source(definition, out, bundle); });
		conditional_write(base_name + "h",    type::stub,      [&](std::ostream & out) { cwcc::generate_stub(out, bundle); });
	}

	void reflect(const char * filename) {
		cwcc::library lib{filename};
		std::istringstream in{lib.definition()};
		const auto bundle = cwcc::parse(in);
		in.seekg(0).clear();
		const auto base_name = cwcc::base_file_name(bundle.name) + '.';

		conditional_write(base_name + "bdl",  type::extracted, [&](std::ostream & out) { out << lib.definition(); });
		conditional_write(base_name + "cwcm", type::extracted, [&](std::ostream & out) { out << lib.metadata(); });
		generate_files(bundle, in);
	}

	void compile(const char * filename) {
		std::ifstream in{filename};
		const auto bundle = cwcc::parse(in);
		in.seekg(0).clear();
		const auto base_name = cwcc::base_file_name(bundle.name) + '.';

		conditional_write(base_name + "cwcm", type::stub, [&](std::ostream & out) { out << "//TODO: {\"Key\", \"Value\"},\n//TODO: {\"Key\", \"Value\"}"; });
		generate_files(bundle, in);
	}
}

int main(int argc, char * argv[]) {
	std::cout << "C++ with Components compiler\n"
	             "© 2012, Michael Florian Hava"
	          << std::endl;
	if(argc != 2) {//batch operations are NOT supported!
		std::cout << "usage:\n"
		             "\tCWCC bundle => .cwch (generated), .cwc (generated), .h (stub), .cwcm (extracted), .bdl (extracted)\n"
		             "\tCWCC bdl    => .cwch (generated), .cwc (generated), .h (stub), .cwcm (stub)"
		          << std::endl;
	} else try {
		const boost::filesystem::path path{argv[1]};
		if(path.extension() != ".bdl") reflect(argv[1]);
		else compile(argv[1]);
	} catch(const std::exception & exc) {
		std::cerr << "error occured while processing \"" << argv[1] << "\":\n" << exc.what() << std::endl;
	}
}