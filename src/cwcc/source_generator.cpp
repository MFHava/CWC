
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
		os << "#include <map>\n"
		      "#include \"" << base_file_name(b.name) << ".h\"\n"
		      "using namespace " << b.name << ";\n";
		std::vector<std::string> components;
		{
			std::vector<std::pair<std::string, std::string>> componentAndExport;
			component_visitor visitor(b.name, componentAndExport);
			for(auto & m : b.members) m.apply_visitor(visitor);
			for(const auto & tmp : componentAndExport) {
				os << "static_assert(std::is_base_of<" << tmp.first << ", " << tmp.second << "$>::value, \"Implementation '" << tmp.second << "$' does not fulfill the requirements of component '" << tmp.first << "'\");\n";
				components.push_back(tmp.second);
			}
		}
		os << "\nstatic const std::map<::cwc::string_ref, ::cwc::intrusive_ptr<cwc::component>> cwc_factories{\n";
		for(auto it{std::begin(components)}; it != std::end(components); ++it) {
			os << "\t{" << *it << "$::cwc_fqn(), cwc::make_intrusive<typename " << *it << "$::cwc_component_factory>()}";
			if(std::next(it) != std::end(components)) os << ',';
			os << "\n";
		}
		os << "};\n"
		      "\n"
		      "static const char * cwc_definition =\n";
		std::transform(std::istream_iterator<line>{is}, std::istream_iterator<line>{}, std::ostream_iterator<std::string>{os}, [](const std::string & str) {
			std::stringstream ss;
			ss << "\t\"";
			for(auto c : str) {
				if(c == '"') ss << '\\';
				ss << c;
			}
			ss << "\\n\"\n";
			return ss.str();
		});//copy content of BDL-file
		os << ";\n"
		      "#include <cwc/internal/bundle.hpp>";
	}
}