
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#include <sstream>
#include <algorithm>
#include "ast.hpp"
#include "generator.hpp"

namespace cwcc {
	namespace {
		void generate_(std::ostream & os, const attribute & a) {
			os << "[[" << a.name;
			if(a.reason) os << "(" << *a.reason << ")";
			os << "]]";
		}

		class vtable_entry final {
			template<bool Definition>
			void generate_vtable(std::ostream & os) const {
				auto first{true}; //TODO: [C++20] merge into for-loop
				for(const auto & p : params) {
					if(first) first = false;
					else os << ", ";
					if(p.const_) os << "const ";
					os << p.type;
					if(p.ref != ref_t::none) os << " *";
					if(Definition) os << " " << p.name;
				}
			}

			const bool ctor{false}, explicit_{false}, const_{false}, delete_, noexcept_{false}, static_{true};
			const ref_t ref{ref_t::none};
			const std::vector<param> & params; //TODO: [C++20] use span
			const std::optional<std::string_view> result;
			const std::string_view name;
		public:
			vtable_entry(const constructor & c) noexcept : ctor{true}, explicit_{c.explicit_}, delete_{c.delete_}, params{c.params}, name{c.name} {}
			vtable_entry(const method & m) noexcept : const_{m.const_}, delete_{m.delete_}, noexcept_{m.noexcept_}, static_{m.static_}, params{m.params}, result{m.result}, name{m.name}, ref{m.ref} {}

			void declaration(std::ostream & os, std::size_t no) const {
				if(delete_) return;
				os << "void(*cwc_" << no << ")(";
				auto first{true};
				if(!noexcept_) {
					os << "cwc::internal::exception *";
					first = false;
				}
				if(!static_) {
					if(first) first = false;
					else os << ", ";
					if(const_) os << "const ";
					os << "void *";
				}
				if(!params.empty()) {
					if(first) first = false;
					else os << ", ";
					generate_vtable<false>(os);
				}
				if(ctor || result) {
					if(!first) os << ", ";
					if(ctor) os << "void **";
					else if(result) os << *result << " *";
				}
				os << ") noexcept;\n";
			}

			void definiton(std::ostream & os) const {
				if(delete_) return;
				os << "+[](";
				auto first{true};
				if(!noexcept_) {
					os << "cwc::internal::exception * cwc_error";
					first = false;
				}
				if(!static_) {
					if(first) first = false;
					else os << ", ";
					if(const_) os << "const ";
					os << "void * cwc_self";
				}
				if(!params.empty()) {
					if(first) first = false;
					else os << ", ";
					generate_vtable<true>(os);
				}
				if(ctor || result) {
					if(!first) os << ", ";
					if(ctor) os << "void **";
					else if(result) os << *result << " *";
					os << " cwc_result";
				}
				os << ") noexcept { ";
				if(!noexcept_) os << "cwc_error->try_([&] { ";
				if(ctor || result) os << "*cwc_result = ";
				if(ctor) os << "new CWCImpl";
				else {
					if(static_) os << "CWCImpl::";
					else {
						if(ref == ref_t::rvalue) os << "std::move";
						os << "reinterpret_cast<";
						if(const_) os << "const ";
						os << "CWCImpl *>(cwc_self)->";
					}
					os << name;
				}
				os << "(";
				first = true; //TODO: [C++20] merge into for-loop
				for(const auto & p : params) {
					if(first) first = false;
					else os << ", ";
					switch(p.ref) {
						case ref_t::none: os << "std::move("; break;
						case ref_t::lvalue: os << "*"; break;
						case ref_t::rvalue: os << "std::move(*"; break;
					}
					os << p.name;
					if(p.ref != ref_t::lvalue) os << ")";
				}
				os << "); ";
				if(!noexcept_) os << "}); ";
				os << "}";
			}

			void wrapper(std::ostream & os, std::size_t no) const {
				if(!ctor) {
					if(static_) os << "static\n";
					os << (result ? "auto" : "void") << " ";
				} else if(explicit_) os << "explicit\n";
				os << name << "(";
				auto first{true}; //TODO: [C++20] merge into for-loop
				for(const auto & p : params) {
					if(first) first = false;
					else os << ", ";
					if(p.const_) os << "const ";
					os << p.type << " ";
					switch(p.ref) {
						case ref_t::lvalue: os << "& "; break;
						case ref_t::rvalue: os << "&& "; break;
					}
					os << p.name;
				}
				os << ") ";
				if(const_) os << "const ";
				switch(ref) {
					case ref_t::lvalue: os << "& ";  break;
					case ref_t::rvalue: os << "&& "; break;
				}
				if(noexcept_) os << "noexcept ";
				if(!ctor && result) os << "-> " << *result << " ";
				if(delete_) os << "=delete;\n";
				else {
					os << "{";
					if(result) os << "\n" << *result << " cwc_result;\n";
					else os << " ";
					os << "cwc_context().call<&cwc_vtable::cwc_" << no << ">(";
					if(!static_) {
						os << "cwc_self";
						if(!params.empty() || result) os << ", ";
					}
					auto first{true}; //TODO: [C++20] merge into for-loop
					for(const auto & p : params) {
						if(first) first = false;
						else os << ", ";
						if(p.ref != ref_t::none) os << "std::addressof(";
						else os << "std::move(";
						os << p.name << ")";
					}
					if(result || ctor) {
						if(!params.empty()) os << ", ";
						if(ctor) os << "&cwc_self";
						else if(result) os << "std::addressof(cwc_result)";
					}
					os << ");";
					if(result) os << "\nreturn cwc_result;\n";
					else os << " ";
					os << "}\n";
				}
				os << "\n";
			}
		};


		template<typename... Ts>
		struct combined final : Ts... {
			using Ts::operator()...;
		};

		template<typename... Ts>
		combined(Ts...) -> combined<Ts...>; //TODO: [C++20] this should be redudant...


		void generate_(std::ostream & os, const param & p) {
			if(p.const_) os << "const ";
			os << p.type << " ";
			switch(p.ref) {
				case ref_t::lvalue: os << "& "; break;
				case ref_t::rvalue: os << "&& "; break;
			}
			os << p.name;
		}

		void generate_(std::ostream & os, const comment & c) {
			os << c.line;
			if(!c.line.empty() && c.line.back() != '\n') os << '\n';
		}

		void generate_(std::ostream & os, const using_ & u) {
			os << "using " << u.name << " = " << u.type << ";\n";
		}

		//TODO: investigate rules of Itanium ABI mangling and apply to mangling logic...
		auto mangle(std::string_view ns, std::string_view type, const std::vector<std::string_view> & tparams = {}) -> std::string { //TODO: [C++20] use span
			//TODO: re-write without all these allocations
			std::string mangled_name, tmp;
			auto append{[&] {
				mangled_name += std::to_string(tmp.size());
				mangled_name += tmp;
				tmp.clear();
			}};
			std::istringstream is{std::string{ns}}; //TODO: [C++23] use ispanstream here
			for(char c; is >> c;) {
				if(c == ':') {
					is >> c;
					assert(c == ':');
					append();
				} else tmp += c;
			}
			append();
			mangled_name += std::to_string(type.size());
			mangled_name += type;

			auto append_tparam{[&] {
				const auto digit{std::isdigit(tmp.front())};
				if(digit) {
					mangled_name += '_';
					mangled_name += tmp;
					tmp.clear();
				} else append();
			}};

			if(!tparams.empty()) {
				mangled_name += 'T';
				for(const auto & t : tparams) {
					std::istringstream is{std::string{t}}; //TODO: [C++23] use ispanstream here
					for(char c; is >> c;) {
						if(c == ':') {
							is >> c;
							assert(c == ':');
							if(!tmp.empty()) append_tparam();
						} else if(c == '<') {
							append_tparam();
							mangled_name += 'T';
						} else if(c == ',') {
							append_tparam();
							mangled_name += 'N';
						} else if(c == '>') {
							append_tparam();
							mangled_name += 'E';
						} else if(!std::isspace(c)) tmp += c;
					}
					append_tparam();
				}
				mangled_name += 'E';
			}
			return mangled_name;
		}

		void generate_(std::ostream & os, const component & c, std::string_view ns, std::variant<const library *, const template_ *> ctx) { //TODO: [C++20] us span
			os << "struct ";
			if(!c.attributes.empty()) {
				for(const auto & a : c.attributes) {
					generate_(os, a);
					os << " ";
				}
			}
			os << c.name << " ";
			if(c.final) os << "final ";
			os << "{\n";
			os << c.name << "(const " << c.name << " &) =delete;\n";
			os << c.name << "(" << c.name << " && cwc_other) noexcept : cwc_self{std::exchange(cwc_other.cwc_self, nullptr)} {}\n";
			os << "auto operator=(const " << c.name << " &) -> " << c.name << " & =delete;\n";
			os << "auto operator=(" << c.name << " && cwc_other) noexcept -> " << c.name << " & { std::swap(cwc_self, cwc_other.cwc_self); return *this; }\n";
			os << "~" << c.name << "() noexcept { cwc_context().call<&cwc_vtable::cwc_0>(cwc_self); }\n";
			os << "\n";

			const auto default_ctor{[&]() -> std::optional<constructor> {
				for(const auto & c : c.content)
					if(std::holds_alternative<constructor>(c))
						return std::nullopt;
				constructor result;
				result.name = c.name;
				return result;
			}()};

			std::size_t no{0}; //TODO: [C++20] merge into for-loop...
			if(default_ctor) vtable_entry{*default_ctor}.wrapper(os, ++no);
			for(const auto & c : c.content)
				std::visit(combined{
					[&](const comment & c) { generate_(os, c); },
					[&](const attribute  & a) { generate_(os, a); os << "\n"; },
					[&](const using_ & u) { generate_(os, u); os << "\n"; },
					[&](const auto & c) { vtable_entry{c}.wrapper(os, ++no); }
				}, c);
			os << "private:\n";
			os << "friend\n";
			os << "cwc::internal::access<" << c.name << ">;\n";
			os << "\n";
			os << "static\n";
			os << "constexpr\n";
			os << "cwc::internal::version cwc_version{" << c.version << "};\n";
			os << "\n";
			os << "struct cwc_vtable final {\n";
			os << "void(*cwc_0)(void *) noexcept;\n";
			no = 0; //TODO: [C++20] merge into for-loop...
			if(default_ctor) vtable_entry{*default_ctor}.declaration(os, ++no);
			for(const auto & c : c.content)
				std::visit(combined{
					[](const comment &) {},
					[](const attribute &) {},
					[](const using_ &) {},
					[&](const auto & c) { vtable_entry{c}.declaration(os, ++no); }
				}, c);
			os << "};\n";
			os << "\n";
			os << "template<typename CWCImpl>\n";
			os << "static\n";
			os << "constexpr\n";
			os << "auto cwc_export() noexcept {\n";
			os << "struct {\n";
			os << "cwc::internal::header h{cwc_version};\n";
			os << "cwc_vtable v{\n";
			os << "+[](void * cwc_self) noexcept { delete reinterpret_cast<CWCImpl *>(cwc_self); }";
			if(default_ctor) vtable_entry{*default_ctor}.definiton(os << ",\n");
			for(const auto & c : c.content)
				std::visit(combined{
					[](const comment &) {},
					[](const attribute &) {},
					[](const using_ &) {},
					[&](const auto & c) { if(!c.delete_) vtable_entry{c}.definiton(os << ",\n"); }
				}, c);
			os << "\n";
			os << "};\n";
			os << "} e;\n";
			os << "return e;\n";
			os << "}\n";
			os << "\n";
			os << "static\n";
			os << "auto cwc_context() -> const cwc::internal::context &";
			const auto mangled{mangle(ns, c.name)};
			std::visit(combined{
				[&](const template_ *) { os << ";\n"; },
				[&](const library * lib) {
					os << " {\n";
					os << "static const cwc::internal::context instance{" << lib->name << ", \"" << mangled << "\", cwc_version};\n";
					os << "return instance;\n";
					os << "}\n";
				}
			}, ctx);
			os << "\n";
			os << "void * cwc_self;\n";
			os << "};\n";
			std::visit(combined{
				[&](const library *) { os << "#define CWC_EXPORT_" << mangled << "(cwc_impl) extern \"C\" CWC_EXPORT const auto cwc_export_" << mangled << "{cwc::internal::access<" << ns << "::" << c.name << ">::template export_<cwc_impl>()}\n"; },
				[](auto) {}
			}, ctx);
		}

		void generate_(std::ostream & os, const template_ & t, std::string_view ns) {
			os << "template<";
			auto first{true}; //TODO: [C++20] merge into for-loop...
			for(const auto & tparam : t.tparams) {
				if(first) first = false;
				else os << ", ";
				os << tparam.type << " " << tparam.name;
			}
			os <<">\n";
			generate_(os, t.component_, ns, &t);
		}

		void generate_(std::ostream & os, const extern_ & e, std::string_view ns, const library & l) {
			const auto mangled{mangle(ns, e.component, e.tparams)};
			os << "#define CWC_EXPORT_" << mangled << "(cwc_impl) extern \"C\" CWC_EXPORT const auto cwc_export_" << mangled << "{cwc::internal::access<" << ns << "::" << e.component << "<";
			auto first{true}; //TODO: [C++20] merge into for-loop...
			for(const auto & t : e.tparams) {
				if(first) first = false;
				else os << ", ";
				os << t;
			}
			os << ">>::template export_<cwc_impl>()}\n";
			os << "template<>\n";
			os << "auto " << e.component << "<";
			first = true; //TODO: [C++20] merge into for-loop...
			for(const auto & t : e.tparams) {
				if(first) first = false;
				else os << ", ";
				os << t;
			}
			os << ">::cwc_context() -> const cwc::internal::context & {\n";
			os << "static const cwc::internal::context instance{" << l.name << ", \"" << mangled << "\", cwc_version};\n";
			os << "return instance;\n";
			os << "}\n";
		}

		void generate_(std::ostream & os, const library & l, std::string_view ns) {
			std::visit(combined{
				[&](const component & c) { generate_(os, c, ns, &l); },
				[&](const extern_ & e) { generate_(os, e, ns, l); }
			}, l.content);
		}

		void generate_(std::ostream & os, const namespace_ & n) {
			os << "namespace " << n.name << " {\n";
			for(const auto & c : n.content) {
				std::visit(combined{
					[&](const comment & c) { generate_(os, c); },
					[&](const template_ & t) { generate_(os, t, n.name); os << "\n"; },
					[&](const library & l) { generate_(os, l, n.name); os << "\n"; }
				}, c);
			}
			os << "}\n";
		}

		void generate_(std::ostream & os, const include & i) {
			os << "#include " << i.header << "\n";
		}
	}

	void generate(std::ostream & os, const cwc & c) {
		os << "//generated with CWCC\n\n";
		os << "#pragma once\n";
		os << "#include <cwc/cwc.hpp>\n";

		auto first{true}; //TODO: [C++20] merge into loop...
		for(const auto & c : c.content) {
			if(first) first = false;
			else os << "\n";
			std::visit([&](const auto & c) { generate_(os, c); }, c);
		}
	}
}
