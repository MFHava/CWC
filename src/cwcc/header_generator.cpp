
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include "line.hpp"
#include "nodes.hpp"
#include "utils.hpp"
#include "indent.hpp"
#include "disclaimer.hpp"
#include "generators.hpp"

namespace cwcc {
	namespace {
		auto name_to_uuid(const bundle & this_bundle, const std::string & s) -> std::string {
			static boost::uuids::name_generator uuid_generator{boost::uuids::string_generator{}("{E4F6FD20-A501-44BB-A840-E9F2B61717F2}")};
			const auto uuid{uuid_generator(this_bundle.name + "::" + s)};
			std::stringstream ss;
			const auto last{std::end(uuid.data) - 1};
			std::copy(std::begin(uuid.data), last, std::ostream_iterator<int>{ss << std::hex << std::uppercase << "0x", ", 0x"});
			ss << static_cast<int>(*last);
			return ss.str();
		}

		auto mangled_names(const bundle & this_bundle, const interface & self) -> std::vector<std::string> {
			std::vector<std::string> result;
			result.reserve(self.methods.size());
			for(std::size_t i{0}; i < self.methods.size(); ++i) {
				auto tmp{remove_double_colon(this_bundle.name + "::" + self.name + "::" + self.methods[i].name + "::")};
				std::replace(std::begin(tmp), std::end(tmp), ':', '$');
				tmp += std::to_string(i);
				result.push_back(std::move(tmp));
			}
			return result;
		}

		auto vtable_entries(const interface & self, const std::vector<std::string> & mangled) -> std::vector<std::string> {
			std::vector<std::string> result;
			for(std::size_t i{0}; i < mangled.size(); ++i) {
				const auto & method{self.methods[i]};
				std::stringstream ss;
				ss << "auto CWC_CALL " << mangled[i] << '(';
				const auto & params{method.params()};
				if(!params.empty()) {
					auto print_param{[&](const param & p) { ss << p.mutable_ << p.type << " * " << p.name; }};
					print_param(params[0]);
					for(std::size_t j{1}; j < params.size(); ++j) {
						ss << ", ";
						print_param(params[j]);
					}
				}
				ss << ") " << method.mutable_ << "noexcept -> const ::cwc::internal::error * ";
				result.push_back(ss.str());
			}
			return result;
		}

		struct header_visitor : boost::static_visitor<> {
			header_visitor(std::ostream & os, const bundle & b) : os{os}, this_bundle{b} {}
			void operator()(const enum_ & self) const { os << self << '\n'; }
			void operator()(const struct_ & self) const { os << self << '\n'; }
			void operator()(const typedef_ & self) const { os << self << '\n'; }
			void operator()(const enumerator & self) const {
				for(const auto & doc : self.lines) os << indent << doc << '\n';
				os << indent << "using " << self.name << " = ::cwc::enumerator<" << self.type << ", " << name_to_uuid(this_bundle, self.name) << ">;\n";
			}
			void operator()(const delegate & self) const {
				for(const auto & doc : self.lines) os << indent << doc << '\n';
				os << indent << "using " << self.name << " = ::cwc::delegate<";
				if(self.out) os << *self.out;
				else os << "void";
				os << "(";

				if(!self.in.empty()) {
					auto print_param{[&](const param & p) { os << p.mutable_ << p.type << " & /*" << p.name << "*/"; }};
					print_param(self.in[0]);
					for(std::size_t i{1}; i < self.in.size(); ++i) {
						os << ", ";
						print_param(self.in[i]);
					}
				}
				os << ")>;\n";
			}
			void operator()(const interface & self) const {
				const auto mangled{mangled_names(this_bundle, self)};
				const auto vtable{vtable_entries(self, mangled)};
				for(const auto & doc : self.lines) os << indent << doc << '\n';
				os << indent << "class " << self.name << " : public ::cwc::component {\n";
				{
					indent_scope scope{os};//TODO: can be merged with loop in C++20
					for(const auto & v : vtable)
						os << indent << "virtual\n"
						   << indent << v << "=0;\n";
				}
				os << indent << "public:\n";
				for(std::size_t i{0}; i < vtable.size(); ++i) {
					indent_scope scope{os};//TODO: why is this nested and not in the outer scope?!
					const auto & method{self.methods[i]};
					for(const auto & doc : method.lines) os << indent << doc << '\n';
					os << indent << (method.out ? "auto" : "void") << " " << method.name << '(';
					if(!method.in.empty()) {
						auto print_param{[&](const param & p) { os << p.mutable_ << p.type << " & " << p.name; }};
						print_param(method.in[0]);
						for(std::size_t j{1}; j < method.in.size(); ++j) {
							os << ", ";
							print_param(method.in[j]);
						}
					}
					os << ") " << method.mutable_;
					if(method.out) os << "-> " << *method.out << ' ';
					os << "{\n";
					{
						indent_scope scope{os};
						if(method.out) os << indent << *method.out << " cwc_ret;\n";
						os << indent << "::cwc::internal::call(*this, &" << self.name << "::" << mangled[i];
						for(const auto & p : method.params()) os << ", " << p.name;
						os << ");\n";
						if(method.out) os << indent << "return cwc_ret;\n";
					}
					os << indent << "}\n";
				}
				os << indent << "};\n";
			}
			void operator()(const component & self) const {
				bundle b;
				b.name = this_bundle.name + "::" + self.name;
				b.members.push_back(self.factory());

				for(const auto & doc : self.lines) os << indent << doc << '\n';
				os << indent << "struct " << self.name << " : " << self.interfaces[0];
				for(std::size_t i{1}; i < self.interfaces.size(); ++i) os << ", " << self.interfaces[i];
				{
					indent_scope scope{os};
					os << " {\n"
					   << indent <<"static\n"
					   << indent <<"auto cwc_fqn() -> ::cwc::string_ref { return \"" << b.name << "\"; }\n"
					   << indent <<"using cwc_interfaces = ::cwc::internal::make_base_list_t<" << self.interfaces[0];
				}
				for(std::size_t i{1}; i < self.interfaces.size(); ++i) os << ", " << self.interfaces[i];
				os << ">;\n\n";
				{
					header_visitor visitor{os, b};
					indent_scope scope{os};//TODO: can be merged with loop in C++20
					for(const auto & m : b.members) m.apply_visitor(visitor);
				}
				os << indent << "};\n";
			}
		private:
			std::ostream & os;
			const bundle & this_bundle;
		};

		struct details_visitor : boost::static_visitor<> {
			details_visitor(std::ostream & os, const bundle & b) : os{os}, this_bundle{b} {}

			void operator()(const enum_ &) const { /*nothing to do*/ }
			void operator()(const struct_ &) const { /*nothing to do*/ }
			void operator()(const typedef_ &) const { /*nothing to do*/ }
			void operator()(const enumerator &) const { /*nothing to do*/ }
			void operator()(const delegate &) const { /*nothing to do*/ }
			void operator()(const interface & self) const {
				os << indent << "template<>\n"
				   << indent << "struct interface_id<::" << this_bundle.name << "::" << self.name << "> final : uuid_constant<" << name_to_uuid(this_bundle, self.name) << "> {};\n"
				                "\n"
				   << indent << "template<typename Implementation, typename TypeList>\n"
				   << indent << "class vtable_implementation<" << this_bundle.name << "::" << self.name << ", Implementation, TypeList> : public ::cwc::internal::default_implementation_chaining<Implementation, TypeList> {\n";
				const auto mangled{mangled_names(this_bundle, self)};
				const auto vtable{vtable_entries(self, mangled)};
				{
					indent_scope scope{os};
					for(std::size_t i{0}; i < vtable.size(); ++i) {
						const auto & method{self.methods[i]};
						os << indent << vtable[i] << "final { return ::cwc::internal::call_and_return_error([&] { ";
						if(method.out) os << "*cwc_ret = ";
						os << "static_cast<" << method.mutable_ << "Implementation &>(*this)." << method.name << '(';
						if(!method.in.empty()) {
							auto print_param{[&](const param & p) { os << '*' << p.name; }};
							print_param(method.in[0]);
							for(std::size_t j{1}; j < method.in.size(); ++j) {
								os << ", ";
								print_param(method.in[j]);
							}
						}
						os << "); }); }\n";
					}
					os << indent << "//detect missing methods:\n";
					std::vector<std::string> methods;
					std::transform(std::begin(self.methods), std::end(self.methods), std::back_inserter(methods), [](const auto & method) { return method.name; });
					methods.erase(std::unique(std::begin(methods), std::end(methods)), std::end(methods));
					for(const auto & method : methods) os << indent << "void " << method << "();\n";
				}
				os << indent << "};\n\n";
			}
			void operator()(const component & self) const {
				bundle b;
				b.name = this_bundle.name + "::" + self.name;
				b.members.push_back(self.factory());
				details_visitor visitor{os, b};
				for(const auto & m : b.members) m.apply_visitor(visitor);
			}
		private:
			std::ostream & os;
			const bundle & this_bundle;
		};

		class dependencies_visitor : public boost::static_visitor<> {
			std::vector<std::string> & dependencies;
		public:
			dependencies_visitor(std::vector<std::string> & dependencies) : dependencies{dependencies} {}

			void operator()(const std::string & self) {
				const auto it{std::find(self.rbegin(), self.rend(), ':').base()};
				if(it == std::begin(self)) return;//not scoped!
				dependencies.emplace_back(std::string(std::begin(self) + 2, it - 2));
			}
			void operator()(const untemplated_type & self) { (*this)(self.name); }
			void operator()(const array & self) { self.type.apply_visitor(*this); }
			void operator()(const array_ref & self) { self.type.apply_visitor(*this); }
			void operator()(const bitset &) { /*nothing to do*/ }
			void operator()(const optional & self) { self.type.apply_visitor(*this); }
			void operator()(const tuple & self) { for(const auto & t : self.types) t.apply_visitor(*this); }
			void operator()(const variant & self) { for(const auto & t : self.types) t.apply_visitor(*this); }
			void operator()(const intrusive_ptr & self) { self.type.apply_visitor(*this); }
			void operator()(const param & self) { self.type.apply_visitor(*this); }
			void operator()(const struct_ & self) { for(const auto & m : self.members) (*this)(m); }
			void operator()(const struct_::member & self) { self.type.apply_visitor(*this); }
			void operator()(const typedef_ & self) { self.type.apply_visitor(*this); }
			void operator()(const interface & self) { for(const auto & m : self.methods) (*this)(m); }
			void operator()(const interface::method & self) { for(const auto & p : self.params()) (*this)(p); }
			void operator()(const enumerator & self) { self.type.apply_visitor(*this); }
			void operator()(const delegate & self) { for(const auto & p : self.in) (*this)(p); }
			void operator()(const component & self) {
				for(const auto & s : self.interfaces) (*this)(s);
				for(const auto & c : self.members) (*this)(c);
			}
			void operator()(const enum_ &) { /*nothing to do*/ }
			void operator()(const component::constructor & self) { for(const auto & p : self.params) (*this)(p); }
		};

		auto dependencies(const bundle & b) -> std::vector<std::string> {
			std::vector<std::string> dependencies;
			dependencies_visitor instance{dependencies};
			for(auto & m : b.members) m.apply_visitor(instance);
			std::sort(std::begin(dependencies), std::end(dependencies));
			dependencies.erase(std::unique(std::begin(dependencies), std::end(dependencies)), std::end(dependencies));
			const std::string tmp[]{"cwc", b.name};
			for(auto & s : tmp)
				if(const auto it{std::find(std::begin(dependencies), std::end(dependencies), s)}; it != std::end(dependencies))
					dependencies.erase(it);
			return dependencies;
		}
	}

	void generate_header(std::ostream & os, const bundle & b) {
		file_disclaimer(os, false);
		os << "#pragma once\n"
		      "#include <cwc/cwc.hpp>\n";
		for(const auto & d : dependencies(b)) os << "#include \"" << base_file_name(d) << ".cwch\"\n";
		os << "\nnamespace " << b.name << " {";
		{
			header_visitor visitor{os, b};
			indent_scope scope{os};//TODO: can be merged with loop in C++20
			for(const auto & m : b.members) {
				os << '\n';
				m.apply_visitor(visitor);
			}
		}
		os << "}\n"
		      "\n"
		      "\n"
		      "//ATTENTION: don't modify the following code, it contains necessary metadata for CWC\n"
		      "namespace cwc::internal {\n";
		details_visitor visitor{os, b};
		indent_scope scope{os}; //TODO: can be merged with loop in C++20
		for(const auto & m : b.members) m.apply_visitor(visitor);
		os << "}\n";
	}
}
