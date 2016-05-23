
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

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
		       "#include <unordered_map>\n"
		       "#include <cwc/internal/metadata_enumerator.hpp>\n"
		       "#include <cwc/internal/create_component_factory.hpp>\n"
		       "using namespace " << b.name << ";\n"
		       "\n"
		       "namespace {\n"
		       "\tusing cwc_components = ::cwc::TL::make_type_list<";
		std::vector<std::pair<std::string, std::string>> componentAndExport;
		component_visitor visitor(b.name, componentAndExport);
		for(auto & m : b.members) m.apply_visitor(visitor);
		if(!componentAndExport.empty()) {
			auto it = std::begin(componentAndExport);
			out << it->second << '$';
			for(++it; it != std::end(componentAndExport); ++it) out << ',' << it->second << '$';

		}
		out << ">::type;\n";
		for(const auto & tmp : componentAndExport) out << "\tstatic_assert(std::is_base_of<" << tmp.first << "::cwc_component, " << tmp.second << "$>::value, \"Implementation '" << tmp.second << "$' does not fulfill the requirements of component '" << tmp.first << "'\");\n";
		out << "\n"
		       "\t::cwc::internal::context_interface * cwc_context;\n"
		       "\tstd::atomic<unsigned long long> cwc_instance_counter{0};\n"
		       "\n"
		       "\tconst std::unordered_multimap<std::string, std::string> cwc_metadata {\n"
		       "#include \"" << base_file_name(b.name) << ".cwcm\"\n"
		       "\t};\n"
		       "\n"
		       "\tclass cwc_metadata_enumerator : public ::cwc::interface_implementation<::cwc::internal::metadata_enumerator, cwc_metadata_enumerator> {\n"
		       "\t\tstd::unordered_multimap<std::string, std::string>::const_iterator it;\n"
		       "\tpublic:\n"
		       "\t\tcwc_metadata_enumerator() : it{std::begin(cwc_metadata)} {}\n"
		       "\n"
		       "\t\tauto end() const -> bool { return it == std::end(cwc_metadata); }\n"
		       "\n"
		       "\t\tvoid next() {\n"
		       "\t\t\tif(end()) throw std::runtime_error{\"already at end\"};\n"
		       "\t\t\t++it;\n"
		       "\t\t}\n"
		       "\n"
		       "\t\tauto get() const -> cwc::internal::metadata_entry {\n"
		       "\t\t\tif(end()) throw std::runtime_error{\"at end\"};\n"
		       "\t\t\treturn{it->first.c_str(), it->second.c_str()};\n"
		       "\t\t}\n"
		       "\t};\n"
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
		       "\t\t::cwc::internal::create_component_factory<cwc_components>::create(fqn, result);\n"
		       "\t});\n"
		       "}\n"
		       "\n"
		       "extern \"C\" CWC_EXPORT ::cwc::internal::error_code CWC_CALL cwc_metadata(::cwc::internal::metadata_enumerator::cwc_interface ** result) {\n"
		       "\tassert(cwc_context);\n"
		       "\tassert(result);\n"
		       "\treturn ::cwc::internal::call_and_return_error([&] {\n"
		       "\t\t::cwc::internal::metadata_enumerator tmp{new cwc_metadata_enumerator};\n"
		       "\t\t*result = ::cwc::internal::to_abi(tmp);\n"
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