
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/spirit/include/karma.hpp>
#include "line.hpp"
#include "nodes.hpp"
#include "disclaimer.hpp"
#include "generators.hpp"
#include "wrap_in_namespace.hpp"

namespace cwcc {
	namespace {
		auto name_to_uuid(const bundle & b, const std::string & s) -> std::string {
			static boost::uuids::name_generator uuid_generator{boost::uuids::string_generator{}("{E4F6FD20-A501-44BB-A840-E9F2B61717F2}")};
			const auto uuid = uuid_generator(b.name + "::" + s);
			std::stringstream ss;
			const auto last = std::end(uuid.data) - 1;
			std::copy(std::begin(uuid.data), last, std::ostream_iterator<int>{ss << std::hex << std::uppercase << "0x", ", 0x"});
			ss << static_cast<int>(*last);
			return ss.str();
		}

		using namespace boost::spirit;

		auto remove_double_colon(std::string s) -> std::string {
			s.erase(std::unique(std::begin(s), std::end(s), [](char c1, char c2) { return c1 == c2 && c1 == ':'; }), std::end(s));
			return s;
		}

		struct header_visitor : boost::static_visitor<> {
			header_visitor(std::ostream & out, const bundle & b) : out{out}, this_bundle{b} {}
			void operator()(const export_ &) { /*nothing to do*/ }
			void operator()(const component & self) {
				bundle b;
				b.name = this_bundle.name + "::" + self.name;
				const auto factory = self.factory();
				b.members.push_back(factory);
				for(const auto & doc : self.lines) out << '\t' << doc << '\n';
				out << "\tstruct " << self.name << " final : ";
				karma::generate(std::ostream_iterator<char>{out}, ("virtual " << karma::string) % ", ", self.interfaces);
				out << " {\n\t\tusing cwc_self = " << self.name << ";\n"
				       "\t\tstruct cwc_component : ";
				karma::generate(std::ostream_iterator<char>{out}, (karma::string << "::cwc_interface") % ", ", self.interfaces);
				out << " {\n\t\t\tstatic auto cwc_fqn() -> ::cwc::ascii_string { return \"" << b.name << "\"; }\n"
				       "\t\t\tusing cwc_interfaces = ::cwc::internal::make_base_list<";
				karma::generate(std::ostream_iterator<char>{out}, (karma::string << "::cwc_interface") % ", ", self.interfaces);
				out << ">::type;\n"
				       "\t\t};\n";
				header_visitor visitor{out, b};
				for(const auto & m : b.members) m.apply_visitor(visitor);
				for(auto tmp : factory.members) {
					tmp.lines.clear();
					for(auto i = 0; i < 2; ++i) {
						for(const auto & doc : tmp.lines) out << "\t\t" << doc << '\n';
						out << "\t\t";
						if(tmp.in.size() == 1) out << "explicit ";
						out << self.name << '(';
						std::stringstream ss;
						external_wrapper_generator{ss, b, factory}(tmp);
						const auto str = ss.str();
						auto first = std::begin(str);
						for(auto i = 0; i < 2; ++i) first = std::find(first, std::end(str), '(') + 1;
						std::copy(first, std::find(first, std::end(str), ')') + 1, std::ostream_iterator<char>{out});
						std::vector<std::string> params;
						std::transform(std::begin(tmp.in), std::end(tmp.in), std::back_inserter(params), [](const param & p) { return p.name; });
						karma::generate(std::ostream_iterator<char>{out}, " : ::cwc::component(::cwc::internal::create_component<cwc_self>(" << -(karma::string % ", ") << ")) {}\n", params);
						//add plugin-related parameter, rest of code stays the same for plugin-constructor (second iteration of this code)
						param plugin;
						plugin.name = "cwc_plugin_id";
						plugin.type = "::cwc::plugin_id";
						tmp.in.insert(std::begin(tmp.in), plugin);
					}
				}
				out << "\n"
				       "\t\t" << self.name << "(const " << self.name << " &) =default;\n"
				       "\t\t" << self.name << "(" << self.name << " &&) noexcept =default;\n"
				       "\t\tauto operator=(const " << self.name << " &) -> " << self.name << " & =default;\n"
				       "\t\tauto operator=(" << self.name << " &&) noexcept -> " << self.name << " & =default;\n"
				       "\t\t~" << self.name << "() noexcept =default;\n"
				       "\t};\n";
			}
			void operator()(const enum_ & self) { out << self << '\n'; }
			void operator()(const struct_ & self) { out << self << '\n'; }
			void operator()(const typedef_ & self) { out << self << '\n'; }
			void operator()(const delegate & self) {
				const interface p = self;
				header_visitor{out, this_bundle}(p);
				out.seekp(-5, std::ios::cur);
				out << "\tprivate:\n"
				       "\t\ttemplate<typename CWCFunctor>\n"
				       "\t\tstruct cwc_delegate : ::cwc::interface_implementation<cwc_self, cwc_delegate<CWCFunctor>> {\n"
				       "\t\t\tCWCFunctor cwc_func;\n"
				       "\t\t\tcwc_delegate(CWCFunctor func) : cwc_func(std::move(func)) {}\n"
				       "\t\t\tvoid operator()(";
				auto first = true;
				for(const auto & in : self.in) {
					if(first) first = false;
					else out << ", ";
					out << in.mutable_ << in.type << " & " << in.name;
				}
				out << ") " << self.mutable_ << "{ cwc_func(";
				first = true;
				for(const auto & in : self.in) {
					if(first) first = false;
					else out << ", ";
					out << "::cwc::internal::from_abi(" << in.name << ')';
				}
				out << "); }\n"
				       "\t\t};\n"
				       "\tpublic:\n"
				       "\t\ttemplate<typename CWCFunctor>\n"
				       "\t\texplicit " << self.name << "(CWCFunctor func) : ::cwc::component(new cwc_delegate<CWCFunctor>(std::move(func))) {}\n"
				       "\t};\n";
			}
			void operator()(const interface & self) {
				for(const auto & doc : self.lines) out << '\t' << doc << '\n';
				out << "\tstruct " << self.name << " : virtual ::cwc::component {\n"
				       "\t\tusing cwc_self = " << self.name << ";\n"
				       "\t\tstruct cwc_interface : ::cwc::component::cwc_interface {\n"
				       "\t\t\tstatic auto cwc_uuid() -> ::cwc::uuid { return {" << name_to_uuid(this_bundle, self.name) << "}; }\n"
				       "\t\t\tusing cwc_wrapper = cwc_self;\n";
				std::for_each(std::begin(self.members), std::end(self.members), vtable_generator{out, this_bundle, self});
				out << "\t\t};\n"
				       "\t\ttemplate<typename CWCImplementation, typename CWCTypeList>\n"
				       "\t\tstruct cwc_interface_implementation : ::cwc::internal::default_implementation_chaining<CWCImplementation, CWCTypeList> {\n";
				std::for_each(std::begin(self.members), std::end(self.members), internal_wrapper_generator{out, this_bundle, self});
				out << "\t\t};\n"
				       "\n"
				       "\t\t" << self.name << "(cwc_interface * ptr) : ::cwc::component{ptr} {}\n"
				       "\t\t" << self.name << "(const cwc_interface * ptr) : ::cwc::component{ptr} {}\n"
				       "\n"
				       "\t\t" << self.name << "(const " << self.name << " &) =default;\n"
				       "\t\t" << self.name << "(" << self.name << " &&) noexcept =default;\n"
				       "\t\tauto operator=(const " << self.name << " &) -> " << self.name << " & =default;\n"
				       "\t\tauto operator=(" << self.name << " &&) noexcept -> " << self.name << " & =default;\n"
				       "\t\t~" << self.name << "() noexcept =default;\n"
				       "\n";
				std::for_each(std::begin(self.members), std::end(self.members), external_wrapper_generator{out, this_bundle, self});
				out << "\tprotected:\n"
				        "\t\t" << self.name << "() {}\n"
				        "\t};\n";
			}
		private:
			std::ostream & out;
			const bundle & this_bundle;

			static auto mangle_name(const bundle & b, const interface & p, const std::string & name, int id) -> std::string {
				auto tmp = remove_double_colon(b.name + "::" + p.name + "::" + (name == "operator()" ? "cwc_operator_invoke" : name) + "::");
				std::replace(std::begin(tmp), std::end(tmp), ':', '$');
				tmp += std::to_string(id);
				return tmp;
			}

			struct vtable_generator {
				vtable_generator(std::ostream & out, const bundle & b, const interface & p) : out{out}, this_bundle{b}, this_interface{p} {}
				void operator()(const interface::method & self) {
					out << "\t\t\tvirtual ::cwc::internal::error_code CWC_CALL " << mangle_name(this_bundle, this_interface, self.name, id++) << '(';
					auto first = true;
					for(const auto & in : self.in) {
						if(first) first = false;
						else out << ", ";
						if(this_interface.name == in.type) out << in.mutable_ << "cwc_interface * " << in.name;
						else out << "::cwc::internal::abi<" << in.mutable_ << in.type << ">::in " << in.name;
						}
					if(self.out) {
						if(!first) out << ", ";
						if(this_interface.name == self.out->type) out << self.out->mutable_ << "cwc_interface ** cwc_ret";
						else out << "::cwc::internal::abi<" << self.out->mutable_ << self.out->type << ">::out cwc_ret";
					}
					out << ") " << self.mutable_ << "=0;\n";
				}
			private:
				std::ostream & out;
				const bundle & this_bundle;
				const interface & this_interface;
				int id{0};
			};

			struct external_wrapper_generator {
				external_wrapper_generator(std::ostream & out, const bundle & b, const interface & p) : out{out}, this_bundle{b}, this_interface{p} {}
				void operator()(const interface::method & self) {
					for(const auto doc : self.lines) out << "\t\t" << doc << '\n';
					out << "\t\t" << (self.out ? "auto" : "void") << ' ' << self.name << '(';
					auto first = true;
					for(const auto & in : self.in) {
						if(first) first = false;
						else out << ", ";
						out << in.mutable_ << in.type << " & " << in.name;
					}
					out << ") " << self.mutable_;
					if(self.out) {
						out << "-> ";
						const auto same_type = this_interface.name == self.out->type;
						if(!same_type) out << "::cwc::internal::abi<";
						out << self.out->mutable_ << self.out->type;
						if(!same_type) out << ">::ret";
						out << ' ';
					}
					out << "{\n";
					if(self.out) {
						const auto same_type = this_interface.name == self.out->type;
						out << "\t\t\t";
						if(!same_type) out << "::cwc::internal::abi<";
						out << self.out->mutable_;
						if(!same_type) out << self.out->type << ">::param";
						else out << "cwc_interface *";
						out << " cwc_ret;\n";
					}
					out << "\t\t\t::cwc::internal::validate(cwc_marshall<cwc_self>()->" << mangle_name(this_bundle, this_interface, self.name, id++) << '(';
					first = true;
					for(const auto & in : self.in) {
						if(first) first = false;
						else out << ", ";
						out << "::cwc::internal::to_abi(" << in.name << ')';
					}
					if(self.out) {
						if(!first) out << ", ";
						out << "::cwc::internal::to_abi(cwc_ret)";
					}
					out << "));\n";
					if(self.out) out << "\t\t\treturn ::cwc::internal::from_abi(cwc_ret);\n";
					out << "\t\t}\n"
					       "\n";
				}
			private:
				std::ostream & out;
				const bundle & this_bundle;
				const interface & this_interface;
				int id{0};
			};

			struct internal_wrapper_generator {
				internal_wrapper_generator(std::ostream & out, const bundle & b, const interface & p) : out{out}, generator{ss, b, p} {}
				void operator()(const interface::method & self) {
					ss.str(""); ss.clear();
					generator(self);
					const auto str = ss.str();
					std::copy(std::begin(str), std::find(str.rbegin(), str.rend(), ')').base(), std::ostream_iterator<char>(out));
					out << ' ' << self.mutable_ << "override final {\n"
					       "\t\t\t\treturn ::cwc::internal::call_and_return_error([&] {\n"
					       "\t\t\t\t\t";
					if(self.out) out << "::cwc::internal::abi<" << self.out->mutable_ << self.out->type << ">::retval cwc_tmp = ";
					out << "static_cast<" << self.mutable_ << "CWCImplementation *>(this)->" << self.name << '(';
					auto first = true;
					for(const auto & in : self.in) {
						if(first) first = false;
						else out << ", ";
						out << "::cwc::internal::from_abi(" << in.name << ')';
					}
					out << ");\n";
					if(self.out) out << "\t\t\t\t\t::cwc::internal::from_abi(cwc_ret) = ::cwc::internal::to_abi(cwc_tmp);\n";
					out << "\t\t\t\t});\n"
					       "\t\t\t}\n";
				}
			private:
				std::ostream & out;
				std::stringstream ss;
				vtable_generator generator;
			};
		};

		struct dependencies_visitor : boost::static_visitor<> {
			std::vector<std::string> & dependencies;

			dependencies_visitor(std::vector<std::string> & dependencies) : dependencies{dependencies} {}

			void add(const std::string & s) {
				const auto it = std::find(s.rbegin(), s.rend(), ':').base();
				if(it == std::begin(s)) return;//not scoped!
				dependencies.emplace_back(std::string(std::begin(s) + 2, it - 2));
			}

			template<typename T>
			void operator()(const T &) { /*ignore*/ }
			void operator()(const export_ & self) { add(self.component); }
			void operator()(const component & self) { for(const auto & s : self.interfaces) add(s); }
			void operator()(const struct_ & self) { for(const auto & s : self.members) (*this)(s); }
			void operator()(const struct_::member & self) { add(self.type); }
			void operator()(const typedef_ & self) { add(self.type); }
			void operator()(const delegate & self) { for(const auto & i : self.in) add(i.type); }
			void operator()(const interface::method & self) {
				if(self.out) (*this)(*self.out);
				for(const auto & i : self.in) (*this)(i);
			}
			void operator()(const param & self) { add(self.type); }
			void operator()(const returns & self) { add(self.type); }
			void operator()(const interface & self) { for(auto & s : self.members) (*this)(s); }
		};

		auto dependencies(const bundle & b) -> std::vector<std::string> {
			std::vector<std::string> dependencies;
			dependencies_visitor instance{dependencies};
			for(auto & m : b.members) m.apply_visitor(instance);
			std::sort(std::begin(dependencies), std::end(dependencies));
			dependencies.erase(std::unique(std::begin(dependencies), std::end(dependencies)), std::end(dependencies));
			const std::string tmp[]{"cwc", b.name};
			for(auto & s : tmp) {
				const auto it = std::find(std::begin(dependencies), std::end(dependencies), s);
				if(it != std::end(dependencies)) dependencies.erase(it);
			}
			return dependencies;
		}
	}

	auto base_file_name(std::string s) -> std::string {
		s = remove_double_colon(std::move(s));
		std::replace(std::begin(s), std::end(s), ':', '.');
		return s;
	}

	void generate_header(std::ostream & out, const bundle & b) {
		disclaimer(out, false);
		out << "#pragma once\n"
		       "#include <cwc/cwc.hpp>\n";
		for(const auto & d : dependencies(b)) out << "#include \"" << base_file_name(d) << ".cwch\"\n";
		out << '\n';
		wrap_in_namespace<header_visitor>(out, b, true, "\n");
	}
}