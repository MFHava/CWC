
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "nodes.hpp"
#include "disclaimer.hpp"
#include "generators.hpp"
#include "wrap_in_namespace.hpp"

namespace cwcc {
	namespace {
		struct stub_visitor : boost::static_visitor<> {
			stub_visitor(std::ostream & out, const bundle & b) : out{out}, this_bundle{b} {
				out << "\t//TODO: add interface implementations\n";
			}
			
			template<typename Ignore>
			void operator()(const Ignore &) {}

			void operator()(const export_ & self) {
				out << "\t//implementation for component '" << self.component << "' - feel free to move this class to any file\n"
				       "\tclass %IMPLEMENTATION_NAME_HERE% : public ::cwc::component_implementation<" << self.component << ", %IMPLEMENTATION_NAME_HERE%\n"
				       "\t\t//TODO: add custom cast table if implementing from more than one interface hierarchy\n"
				       "\t\t//::cwc::make_type_list<\n"
				       "\t\t//\tstd::pair<%AMBIGUOUS_INTERFACE_0%, %UNAMBIGUOUS_INTERFACE_0%>,\n"
				       "\t\t//\tstd::pair<%AMBIGUOUS_INTERFACE_1%, %UNAMBIGUOUS_INTERFACE_1%>\n"
				       "\t\t//>\n"
				       "\t> {\n"
				       "\t\t//TODO: members\n"
				       "\tpublic:\n"
				       "\t\t//TODO: constructors\n"
				       "\t\t//TODO: all interface methods except those from ::cwc::component\n"
				       "\t};\n"
				       "\n"
				       "\tusing " << self.component << "$ = %IMPLEMENTATION_NAME_HERE%;//binding your implementation to the name used by the CWC-system\n";
			}

		private:
			std::ostream & out;
			const bundle & this_bundle;
		};
	}

	void generate_stub(std::ostream & out, const bundle & b) {
		file_disclaimer(out, true);
		out << "//ATTENTION: stub file for CWC component implementation - by convention the expected filename will be '" << base_file_name(b.name) << ".h'\n"
		       "#pragma once\n"
		       "#include \"" << base_file_name(b.name) << ".cwch\"\n"
		       "//TODO: add additional includes\n"
		       "\n";
		wrap_in_namespace<stub_visitor>(out, b, false);
	}
}