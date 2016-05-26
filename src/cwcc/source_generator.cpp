
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <map>
#include <sstream>
#include "line.hpp"
#include "nodes.hpp"
#include "disclaimer.hpp"
#include "generators.hpp"

namespace cwcc {
	namespace {
		struct component_visitor : boost::static_visitor<> {
			component_visitor(const std::string & bundle, std::vector<std::pair<std::string, std::string>> & componentAndExport) : bundle{bundle}, componentAndExport{componentAndExport} {}

			void operator()(const export_ & self) {
				const auto it = std::find(self.component.rbegin(), self.component.rend(), ':').base();
				componentAndExport.push_back(std::make_pair(self.component, bundle + "::" + std::string(it, std::end(self.component))));
			}

			template<typename Ignore>
			void operator()(const Ignore &) {}

		private:
			const std::string & bundle;
			std::vector<std::pair<std::string, std::string>> & componentAndExport;
		};
	}

	void generate_source(std::istream & in, std::ostream & out, const bundle & b) {
		disclaimer(out, false);
		out << "#include \"" << base_file_name(b.name) << ".h\"\n"
		       "#include <cstring>\n"
		       "using namespace " << b.name << ";\n"
		       "\n"
		       "namespace {\n";
		std::map<std::size_t, std::vector<std::string>> components;
		{
			std::vector<std::pair<std::string, std::string>> componentAndExport;
			component_visitor visitor(b.name, componentAndExport);
			for(auto & m : b.members) m.apply_visitor(visitor);
			for(const auto & tmp : componentAndExport) {
				out << "\tstatic_assert(std::is_base_of<" << b.name << "::" << tmp.first << "::cwc_component, " << tmp.second << "$>::value, \"Implementation '" << tmp.second << "$' does not fulfill the requirements of component '" << tmp.first << "'\");\n";
				components[tmp.second.size()].push_back(tmp.second);
			}
		}
		out << "\n"
		       "\t::cwc::internal::context_interface * cwc_context;\n"
		       "\tstd::atomic<unsigned long long> cwc_instance_counter{0};\n"
		       "}\n"
		       "\n"
		       "auto ::cwc::internal::this_context() -> context_interface & { return *cwc_context; }\n"
		       "::cwc::internal::instance_counter::instance_counter() { ++cwc_instance_counter; }\n"
		       "::cwc::internal::instance_counter::~instance_counter() { --cwc_instance_counter; }\n"
		       "\n"
		       "extern \"C\" CWC_EXPORT::cwc::boolean CWC_CALL cwc_unloadable() { return !cwc_instance_counter; }\n"
		       "\n"
		       "extern \"C\" CWC_EXPORT void CWC_CALL cwc_init(::cwc::internal::context_interface * context) {\n"
		       "\tassert(!cwc_context);\n"
		       "\tcwc_context = context;\n"
		       "}\n"
		       "\n"
		       "extern \"C\" CWC_EXPORT ::cwc::internal::error_code CWC_CALL cwc_factory(::cwc::ascii_string fqn, cwc::component::cwc_interface ** result) {\n"
		       "\tassert(cwc_context);\n"
		       "\tassert(result);\n"
		       "\treturn ::cwc::internal::call_and_return_error([&] {\n"
		       "\t\tswitch(std::strlen(fqn)) {\n";
		for(const auto & pair : components) {
			out << "\t\t\tcase " << pair.first << ": {\n";
			for(auto & fqn : pair.second) out << "\t\t\t\tif(!std::strcmp(fqn, " << fqn << "$::cwc_fqn())) return *result = new typename " << fqn << "$::cwc_component_factory;\n";
			out << "\t\t\t} break;\n";
		}
		out << "\t\t}\n"
		       "\t\tthrow std::logic_error{\"unsupported component\"};\n"
		       "\t});\n"
		       "}\n"
		       "\n"
		       "extern \"C\" CWC_EXPORT ::cwc::ascii_string CWC_CALL cwc_reflect() {\n"
		       "\treturn\n";
		std::transform(std::istream_iterator<line>{in}, std::istream_iterator<line>{}, std::ostream_iterator<std::string>{out}, [](const std::string & str) {
			std::stringstream ss;
			ss << "\t\t\"";
			for(auto c : str) {
				if(c == '"') ss << '\\';
				ss << c;
			}
			ss << "\\n\"\n";
			return ss.str();
		});//copy content of BDL-file
		out << "\t;\n"
		       "}";
	}
}