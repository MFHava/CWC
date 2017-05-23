
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "nodes.hpp"
#include "utils.hpp"
#include "disclaimer.hpp"
#include "generators.hpp"
#include "wrap_in_namespace.hpp"

namespace cwcc {
	namespace {
		struct stub_visitor : boost::static_visitor<> {
			stub_visitor(std::ostream & os, const bundle & b) : os{os}, this_bundle{b} {
				os << "\t//TODO: add interface implementations\n";
			}
			
			template<typename Ignore>
			void operator()(const Ignore &) {}

			void operator()(const export_ & self) {
				os << "\t//implementation for component '" << self.component << "' - feel free to move this class to any file\n"
				      "\tclass %IMPLEMENTATION_NAME_HERE% : public ::cwc::component_implementation<" << self.component << ", %IMPLEMENTATION_NAME_HERE%> {\n"
				      "\t\t//TODO: members\n"
				      "\tpublic:\n"
				      "\t\t//TODO: constructors\n"
				      "\t\t//TODO: all interface methods except those from ::cwc::component\n"
				      "\t};\n"
				      "\n"
				      "\tusing " << self.component << "$ = %IMPLEMENTATION_NAME_HERE%;//binding your implementation to the name used by the CWC-system\n";
			}

		private:
			std::ostream & os;
			const bundle & this_bundle;
		};
	}

	void generate_stub(std::ostream & os, const bundle & b) {
		file_disclaimer(os, true);
		os << "//ATTENTION: stub file for CWC component implementation - by convention the expected filename will be '" << base_file_name(b.name) << ".h'\n"
		      "#pragma once\n"
		      "#include \"" << base_file_name(b.name) << ".cwch\"\n"
		      "//TODO: add additional includes\n"
		      "\n";
		wrap_in_namespace<stub_visitor>(os, b, false);
	}
}