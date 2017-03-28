
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <map>
#include <sstream>
#include "line.hpp"
#include "nodes.hpp"
#include "utils.hpp"
#include "disclaimer.hpp"
#include "generators.hpp"

namespace cwcc {
	namespace {
		struct component_visitor : boost::static_visitor<> {
			component_visitor(const std::string & bundle, std::vector<std::pair<std::string, std::string>> & componentAndExport) : bundle{bundle}, componentAndExport{componentAndExport} {}

			void operator()(const export_ & self) {
				const auto it = std::find(self.component.rbegin(), self.component.rend(), ':').base();
				assert(!self.component.empty());
				auto tmp = self.component.front() == ':' ? self.component : "::" + bundle + "::" + self.component;//iff contract is local identifier: prepend with namespace, otherwise it is already fully qualified
				componentAndExport.push_back(std::make_pair(std::move(tmp), bundle + "::" + std::string(it, std::end(self.component))));
			}

			template<typename Ignore>
			void operator()(const Ignore &) {}

		private:
			const std::string & bundle;
			std::vector<std::pair<std::string, std::string>> & componentAndExport;
		};
	}

	void generate_source(std::istream & is, std::ostream & os, const bundle & b) {
		file_disclaimer(os, false);
		os << "#include \"" << base_file_name(b.name) << ".h\"\n"
		      "using namespace " << b.name << ";\n"
		      "\n"
		      "namespace {\n";
		std::map<std::size_t, std::vector<std::string>> components;//TODO: could be replaced with a multimap
		{
			std::vector<std::pair<std::string, std::string>> componentAndExport;
			component_visitor visitor(b.name, componentAndExport);
			for(auto & m : b.members) m.apply_visitor(visitor);
			for(const auto & tmp : componentAndExport) {
				os << "\tstatic_assert(std::is_base_of<" << tmp.first << ", " << tmp.second << "$>::value, \"Implementation '" << tmp.second << "$' does not fulfill the requirements of component '" << tmp.first << "'\");\n";
				components[tmp.first.size()].push_back(tmp.second);
			}
		}
		os << "\n"
		      "\t::cwc::intrusive_ptr<::cwc::context> cwc_context;\n"
		      "}\n"
		      "\n"
		      "auto ::cwc::this_context() -> ::cwc::intrusive_ptr<context> { return cwc_context; }\n"
		      "\n"
		      "extern \"C\" CWC_EXPORT void CWC_CALL cwc_init(::cwc::intrusive_ptr<::cwc::context> context) {\n"
		      "\tassert(!cwc_context);\n"
		      "\tcwc_context = context;\n"
		      "}\n"
		      "\n"
		      "extern \"C\" CWC_EXPORT ::cwc::internal::error_code CWC_CALL cwc_factory(const ::cwc::string_ref * fqn, cwc::intrusive_ptr<cwc::component> * result) {\n"
		      "\tassert(cwc_context);\n"
		      "\tassert(result);\n"
		      "\treturn ::cwc::internal::call_and_return_error([&] {\n"
		      "\t\tswitch(fqn->size()) {\n";
		for(const auto & pair : components) {
			os << "\t\t\tcase " << (pair.first - 2) << ": {\n";//fqns are NOT prepended with double colons, but contain \0
			for(auto & fqn : pair.second) os << "\t\t\t\tif(*fqn == " << fqn << "$::cwc_fqn()) return *result = cwc::make_intrusive<typename " << fqn << "$::cwc_component_factory>();\n";
			os << "\t\t\t} break;\n";
		}
		os << "\t\t}\n"
		      "\t\tthrow std::logic_error{\"unsupported component\"};\n"
		      "\t});\n"
		      "}\n"
		      "\n"
		      "extern \"C\" CWC_EXPORT void CWC_CALL cwc_reflect(::cwc::string_ref * definition) {\n"
		      "\t*definition =\n";
		std::transform(std::istream_iterator<line>{is}, std::istream_iterator<line>{}, std::ostream_iterator<std::string>{os}, [](const std::string & str) {
			std::stringstream ss;
			ss << "\t\t\"";
			for(auto c : str) {
				if(c == '"') ss << '\\';
				ss << c;
			}
			ss << "\\n\"\n";
			return ss.str();
		});//copy content of BDL-file
		os << "\t;\n"
		      "}";
	}
}