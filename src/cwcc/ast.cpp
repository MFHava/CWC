
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include "ast.hpp"
#include "parser.hpp"

//TODO: extensive error messages
//TODO: split AST from parsing and code generation

namespace cwcc {
	attribute_list::attribute_list(parser & p) {
		//TODO: [C++23] all attributes are allowed to appear multiple times in a declaration
		while(p.consume("[[")) {
retry:
			if(p.consume("deprecated")) {
				if(deprecated) throw std::logic_error{"detected duplicated deprecated-attribute"};
				if(p.consume("(")) {
					deprecated = p.next(type::string);
					p.expect(")");
				} else deprecated = "";
			} else {
				p.expect("nodiscard");
				if(nodiscard) throw std::logic_error{"detected duplicated nodiscard-attribute"};
				if(p.consume("(")) {
					nodiscard = p.next(type::string);
					p.expect(")");
				} else nodiscard = "";
			}
			if(p.consume(",")) goto retry;
			p.expect("]]");
		}
	}

	void attribute_list::generate(std::ostream & os) const {
		os << "[[";
		auto func{[&, first{true}](const char * name, const auto & value) mutable {
			if(!value) return;
			if(!first) os << ", ";
			os << name;
			if(value->size() > 2) os << "(" << *value << ")"; //collapse empty comments (provided by parser as "")
			first = false;
		}};

		func("nodiscard", nodiscard);
		func("deprecated", deprecated);
		os << "]]";
	}


	param_list::param::param(parser & p) {
		const_ = p.consume("const");
		type = p.next(type::type);
		if(const_) {
			p.expect("&");
			ref = ref_t::lvalue;
		} else {
			if(p.consume("&")) ref = ref_t::lvalue;
			else if(p.consume("&&")) ref = ref_t::rvalue;
			else ref = ref_t::none;
		}
		name = p.next(type::name);
		if(p.starts_with('/')) trailing_comment = p.next(type::comment);
	}

	void param_list::param::generate_param(std::ostream & os) const {
		if(const_) os << "const ";
		os << type << " ";
		switch(ref) {
			case ref_t::lvalue: os << "& "; break;
			case ref_t::rvalue: os << "&& "; break;
		}
		os << name;
	}

	param_list::param_list(parser & p) {
		params.emplace_back(p);
		while(!p.accept(")")) {
			p.expect(",");
			params.emplace_back(p);
		}
	}

	void param_list::generate_param_passing(std::ostream & os) const {
		auto first{true};
		for(const auto & p : params) {
			if(first) first = false;
			else os << ", ";
			if(p.ref != ref_t::none) os << "std::addressof(";
			else os << "std::move(";
			os << p.name << ")";
		}
	}

	void param_list::generate_param_list(std::ostream & os) const {
		auto split_list{false};
		for(const auto & p : params)
			if(!p.trailing_comment.empty()) {
				split_list = true;
				break;
			}
		if(split_list) os << "\n";
	
		for(std::size_t i{0}; i < params.size(); ++i) {
			const auto & p{params[i]};
			if(p.const_) os << "const ";
			os << p.type << " ";
			switch(p.ref) {
				case ref_t::lvalue: os << "& "; break;
				case ref_t::rvalue: os << "&& "; break;
			}
			os << p.name;
			if(i + 1 != params.size()) {
				os << ",";
				if(!split_list) os << " ";
			}
			if(!p.trailing_comment.empty()) os << " " << p.trailing_comment;
			if(split_list) os << "\n";
		}
	}

	void param_list::generate_vtable(std::ostream & os, bool definition) const {
		auto first{true};
		for(const auto & p : params) {
			if(first) first = false;
			else os << ", ";
			if(p.const_) os << "const ";
			os << p.type;
			if(p.ref != ref_t::none) os << " *";
			if(definition) os << " " << p.name;
		}
	}

	void param_list::generate_vtable_declaration(std::ostream & os) const { generate_vtable(os, false); }
	void param_list::generate_vtable_definition(std::ostream & os) const { generate_vtable(os, true); }

	void param_list::generate_vtable_definition_paramters(std::ostream & os) const {
		auto first{true};
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
	}


	comment_list::comment_list(parser & p) { while(p.starts_with('/')) comments.emplace_back(p.next(type::comment)); }

	void comment_list::generate(std::ostream & os) const { for(const auto & c : comments) os << c << '\n'; }


	constructor::constructor(parser & p, std::optional<comment_list> clist, std::optional<attribute_list> alist) : clist{std::move(clist)}, alist{std::move(alist)} {
		p.expect("(");
		if(!p.accept(")")) plist = p;
		p.expect(")");
		if(delete_ = p.consume("=")) p.expect("delete");
	}

	void constructor::generate_definition(std::ostream & os, std::string_view class_, std::size_t no) const {
		os << "\n";
		if(clist) clist->generate(os);
		if(alist) {
			alist->generate(os);
			os << "\n";
		}
		os << class_ << "(";
		if(plist) plist->generate_param_list(os);
		os << ") ";
		if(delete_) os << "=delete;\n";
		else {
			os << "{ cwc_context().call<&cwc_vtable::cwc_" << no << ">(";
			if(plist) {
				plist->generate_param_passing(os);
				os << ", ";
			}
			os << "&cwc_self";
			os << "); }\n";
		}
	}

	void constructor::generate_vtable_declaration(std::ostream & os, std::size_t no) const {
		if(delete_) return;
		os << "cwc::internal::error_callback(CWC_CALL * cwc_" << no << ")(";
		if(plist) {
			plist->generate_vtable_declaration(os);
			os << ", ";
		}
		os << "void **) noexcept;\n";
	}

	void constructor::generate_vtable_definition(std::ostream & os, bool last) const {
		if(delete_) return;
		os << "+[](";
		if(plist) {
			plist->generate_vtable_definition(os);
			os << ", ";
		}
		os << "void ** cwc_result) noexcept { return cwc::internal::try_([&] { *cwc_result = new cwc_impl{";
		if(plist) plist->generate_vtable_definition_paramters(os);
		os << "}; }); }";
		if(!last) os << ",";
		os << "\\\n";
	}


	method::method(parser & p, std::optional<comment_list> clist, std::optional<attribute_list> alist) : clist{std::move(clist)}, alist{std::move(alist)} {
		static_ = p.consume("static");
		const auto returning{p.consume("auto")};
		if(!returning) p.expect("void");

		if(p.consume("operator")) {
			p.expect("(");
			p.expect(")");
			name = "operator()";
		} else name = p.next(type::name);
		p.expect("(");
		if(!p.accept(")")) plist = p;
		p.expect(")");
		if(static_) {
			const_ = false;
			ref = ref_t::none;
		} else {
			const_ = p.consume("const");
			if(p.consume("&")) ref = ref_t::lvalue;
			else if(p.consume("&&")) ref = ref_t::rvalue;
			else ref = ref_t::none;
		}
		noexcept_ = p.consume("noexcept");
		if(returning) {
			p.expect("->");
			result = p.next(type::type);
		}
		if(delete_ = p.consume("=")) p.expect("delete");
	}

	void method::generate_definition(std::ostream & os, std::size_t no) const {
		os << "\n";
		if(clist) clist->generate(os);
		if(alist) {
			alist->generate(os);
			os << "\n";
		}
		if(static_) os << "static\n";
		os << (result ? "auto" : "void") << " " << name << "(";
		if(plist) plist->generate_param_list(os);
		os << ") ";
		if(const_) os << "const ";
		switch(ref) {
			case ref_t::lvalue: os << "& ";  break;
			case ref_t::rvalue: os << "&& "; break;
		}
		if(noexcept_) os << "noexcept ";
		if(result) os << "-> " << *result << " ";
		if(delete_) os << "=delete;\n";
		else {
			os << "{";
			if(result) os << "\n" << *result << " cwc_result;\n";
			else os << " ";
			os << "cwc_context().call<&cwc_vtable::cwc_" << no << ">(";
			if(!static_) {
				os << "cwc_self";
				if(plist || result) os << ", ";
			}
			if(plist) plist->generate_param_passing(os);
			if(result) {
				if(plist) os << ", ";
				os << "std::addressof(cwc_result)";
			}
			os << ");";
			if(result) os << "\nreturn cwc_result;\n";
			else os << " ";
			os << "}\n";
		}
	}

	void method::generate_vtable_declaration(std::ostream & os, std::size_t no) const {
		if(delete_) return;
		if(noexcept_) os << "void";
		else os << "cwc::internal::error_callback";
		os << "(CWC_CALL * cwc_" << no << ")(";
		if(!static_) {
			if(const_) os << "const ";
			os << "void *";
			if(plist || result) os << ", ";
		}
		if(plist) plist->generate_vtable_declaration(os);
		if(result) {
			if(plist) os << ", ";
			os << *result << " *";
		}
		os << ") noexcept;\n";
	}

	void method::generate_vtable_definition(std::ostream & os, bool last) const {
		if(delete_) return;
		os << "+[](";
		if(!static_) {
			if(const_) os << "const ";
			os << "void * cwc_self";
			if(plist || result) os << ", ";
		}
		if(plist) plist->generate_vtable_definition(os);
		if(result) {
			if(plist) os << ", ";
			os << *result << " * cwc_result";
		}
		os << ") noexcept { ";
		if(!noexcept_) os << "return cwc::internal::try_([&] { ";
		if(result) os << "*cwc_result = ";
		if(static_) os << "cwc_impl::";
		else {
			if(ref == ref_t::rvalue) os << "std::move";
			os << "reinterpret_cast<";
			if(const_) os << "const ";
			os << "cwc_impl *>(cwc_self)->";
		}
		os << name << "(";
		if(plist) plist->generate_vtable_definition_paramters(os);
		os << "); ";
		if(!noexcept_) os << "}); ";
		os << "}";
		if(!last) os << ",";
		os << "\\\n";
	}


	component::component(parser & p) {
		if(p.starts_with('/')) clist = p;
		p.expect("@library");
		p.expect("(");
		dll = p.next(type::string);
		p.expect(")");
		p.expect("component");
		if(p.accept("[[")) alist = p;
		name = p.next(type::name);
		final_ = p.consume("final");
		p.expect("{");
		while(!p.consume("}")) {
			std::optional<comment_list> clist;
			if(p.starts_with('/')) clist = p;
			std::optional<attribute_list> alist;
			if(p.accept("[[")) alist = p;
			if(p.consume(name)) constructors.emplace_back(p, std::move(clist), std::move(alist));
			else methods.emplace_back(p, std::move(clist), std::move(alist));
			p.expect(";");
		}
		p.expect(";");

		if(constructors.empty()) constructors.emplace_back(); //like normal C++, if no custom ctor is provided, a default constructor is generated
	}

	void component::generate(std::ostream & os, const std::string & namespace_) const {
		const auto mangled_name{[&] {
			std::string mangled_name;
			std::istringstream is{namespace_};
			for(char c; is >> c;) {
				if(c == ':') {
					is >> c;
					mangled_name += '$';
				} else mangled_name += c;
			}
			mangled_name += '$';
			mangled_name += name;
			return mangled_name;
		}()};
		if(clist) clist->generate(os);
		os << "struct ";
		if(alist) {
			alist->generate(os);
			os << " ";
		}
		os << name << " ";
		if(final_) os << "final ";
		os << "{\n";
		os << name << "(const " << name << " &) =delete;\n";
		os << name << "(" << name << " && cwc_other) noexcept : cwc_self{std::exchange(cwc_other.cwc_self, nullptr)} {}\n";
		os << "auto operator=(const " << name << " &) -> " << name << " & =delete;\n";
		os << "auto operator=(" << name << " && cwc_other) noexcept -> " << name << " & { std::swap(cwc_self, cwc_other.cwc_self); return *this; }\n";
		os << "~" << name << "() noexcept { cwc_context().call<&cwc_vtable::cwc_0>(cwc_self); }\n";
		os << "\n";
		{
			std::size_t no{1};
			for(const auto & c : constructors) c.generate_definition(os, name, no++);
			for(const auto & m : methods) m.generate_definition(os, no++);
		}
		os << "private:\n";
		os << "friend\n";
		os << "cwc::internal::access<" << name << ">;\n";
		os << "\n";
		os << "struct cwc_vtable final {\n";
		os << "void(CWC_CALL * cwc_0)(void *) noexcept;\n";
		{
			std::size_t no{1};
			for(const auto & c : constructors) c.generate_vtable_declaration(os, no++);
			for(const auto & m : methods) m.generate_vtable_declaration(os, no++);
		}
		os << "};\n";
		os << "\n";
		os << "static\n";
		os << "auto cwc_context() -> const cwc::internal::context & {\n";
		os << "static const cwc::internal::context instance{" << dll << ", \"" << mangled_name << "\"};\n";
		os << "return instance;\n";
		os << "}\n";
		os << "\n";
		os << "void * cwc_self;\n";
		os << "};\n";
		os << "#define CWC_EXPORT_" << mangled_name << "(cwc_impl)\\\n";
		os << "extern \"C\" CWC_EXPORT const typename cwc::internal::access<" << namespace_ << "::" << name << ">::vtable " << mangled_name << "{\\\n";
		os << "+[](void * cwc_self) noexcept { delete reinterpret_cast<cwc_impl *>(cwc_self); },\\\n";
		{
			const auto count{constructors.size() + methods.size()};
			std::size_t no{0};
			for(const auto & c : constructors) c.generate_vtable_definition(os, ++no == count);
			for(const auto & m : methods) m.generate_vtable_definition(os, ++no == count);
		}
		os << "}\n";
	}


	namespace_::namespace_(parser & p) {
		if(p.starts_with('/')) clist = p;
		p.expect("namespace");
		name = p.next(type::nested);
		p.expect("{");
		while(!p.consume("}")) components.emplace_back(p);
	}

	void namespace_::generate(std::ostream & os) const {
		if(clist) clist->generate(os);
		os << "namespace " << name << " {\n";
		auto first{true};
		for(const auto & c : components) {
			if(first) first = false;
			else os << "\n\n";
			c.generate(os, name);
		}
		os << "}\n";
	}


	cwcc::cwcc(parser & p) { while(p.has_tokens()) namespaces.emplace_back(p); }

	void cwcc::generate(std::ostream & os) const {
		os << "//generated with CWCC\n\n";
		os << "#pragma once\n";
		os << "#include <cwc/cwc.hpp>\n\n";

		auto first{true};
		for(const auto & n : namespaces) {
			if(first) first = false;
			else os << "\n\n";
			n.generate(os);
		}
	}
}
