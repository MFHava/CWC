#include <queue>
#include <cassert>
#include <vector>
#include <string>
#include <fstream>
#include <optional>
#include <iterator>
#include <stdexcept>
#include <sstream>
#include <string_view>
#include <iostream>
#include <algorithm>

/* GRAMMAR

CWC             =      NAMESPACE*;
NAMESPACE       =      COMMENT* "namespace" NESTED_NAME '{' COMPONENT* '}'
COMPONENT       =      COMMENT* ATTRIBUTES "component" NAME '{' BODY* '}' ';'
BODY            =      CONSTRUCTOR | METHOD
CONSTRUCTOR     =      COMMENT* NAME ARG_LIST
METHOD          =      COMMENT* ("void" | "auto") NAME ARG_LIST CONST "->" TYPE ';'
ARG_LIST        =      '(' ARGS ')'
ARGS            =      ARG (',' ARG)*
ARG             =      CONST TYPE '&' NAME COMMENT
CONST           =      "const"?
COMMENT         =      "//" .* \n
ATTRIBUTES      =      "[[" "library" '=' LIB_NAME "]]"
TYPE            =      NESTED_NAME
NAME            =      (a-zA-Z)(a-zA-Z0-9_)*
NESTED_NAME     =      NAME("::"NAME)*
LIB_NAME        =      (a-zA-Z0-9_-)+
*/

class parser final {
	std::queue<std::string> tokens;

	static
	void throw_stray(char c) {
		std::string msg{"detected stray '"};
		msg += c;
		msg += '\'';
		throw std::logic_error{msg};
	}
public:
	parser(std::istream & in) {
		in >> std::noskipws;
		//TODO: error handling

		for(char c; in >> c;) {
retry:
			if(std::isspace(c)) continue;
			switch(c) {
				case '/': {
					in >> c;
					if(c == '/') {
						std::string line{"//"};
						while(in >> c) {
							if(c == '\n') break;
							line += c;
						}
						tokens.push(std::move(line));
					} else throw_stray('/');
				} break;
				case '-': {
					in >> c;
					if(c == '>') tokens.push("->");
					else throw_stray('-');
				} break;
				case '[': {
					in >> c;
					if(c == '[') tokens.push("[[");
					else throw_stray('[');
				} break;
				case ']': {
					in >> c;
					if(c == ']') tokens.push("]]");
					else throw_stray(']');
				} break;
				case '&':
				case '(':
				case ')':
				case '{':
				case '}':
				case ',':
				case ';':
				case '=':
					tokens.push(std::string(1, c));
					break;
				default: {
					if(std::isalpha(c)) {
						std::string token(1, c);
						while(in >> c) {
							if(c == '_' || c == '-' || std::isalnum(c)) token += c; //TODO: '-' is only valid when state allows it
							else if(c == ':') {
								in >> c;
								if(c == ':') token += "::";
								else throw_stray(':');
							}
							else {
								tokens.push(std::move(token));
								goto retry;
							}
						}
					} else throw_stray(c);
				} break;
			}
		}
	}

	void expect(std::string_view str) {
		if(accept(str)) tokens.pop();
		else {
			std::stringstream ss;
			ss << "expected \"" << str << "\" - ";
			if(tokens.empty()) ss << "EOF reached";
			else ss << "encountered \"" << tokens.front() << "\"";
			throw std::invalid_argument{ss.str()};
		}
	}

	auto accept(std::string_view str) const -> bool {
		if(tokens.empty()) return false;
		return tokens.front() == str;
	}

	auto consume(std::string_view str) -> bool {
		if(accept(str)) {
			tokens.pop();
			return true;
		} else return false;
	}

	auto next() -> std::string {
		std::string result;
		result = std::move(tokens.front());
		tokens.pop();
		return result;
	}

	auto starts_with(char ch) const -> bool {
		if(tokens.empty()) return false;
		return tokens.front()[0] == ch;
	}

	auto has_tokens() const noexcept -> bool { return !tokens.empty(); }
};


class param_list final {
	struct param final {
		bool const_;
		std::string type, name, trailing_comment;

		param(parser & p) {
			const_ = p.consume("const");
			type = p.next();
			p.expect("&");
			name = p.next();
			if(p.starts_with('/')) trailing_comment = p.next();
		}

		void generate_param(std::ostream & os) const {
			if(const_) os << "const ";
			os << type << " & " << name;
		}
	};

	std::vector<param> params;

	template<bool Definition>
	void generate_vtable(std::ostream & os) const {
		auto first{true};
		for(const auto & p : params) {
			if(first) first = false;
			else os << ", ";
			if(p.const_) os << "const ";
			os << p.type << " *";
			if constexpr(Definition) os << " " << p.name;
		}
	}
public:
	param_list() =default;
	param_list(parser & p) {
		p.expect("(");
		if(!p.consume(")")) {
			params.emplace_back(p);
			while(!p.consume(")")) {
				p.expect(",");
				params.emplace_back(p);
			}
		}
	}

	void generate_param_passing(std::ostream & os) const {
		auto first{true};
		for(const auto & p : params) {
			if(first) first = false;
			else os << ", ";
			os << "std::addressof(" << p.name << ")";
		}
	}

	void generate_param_list(std::ostream & os) const {
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
			os << p.type << " & " << p.name;
			if(i + 1 != params.size()) {
				os << ",";
				if(!split_list) os << " ";
			}
			if(!p.trailing_comment.empty()) os << " " << p.trailing_comment;
			if(split_list) os << "\n";
		}
	}

	void generate_vtable_declaration(std::ostream & os) const { generate_vtable<false>(os); }
	void generate_vtable_definition(std::ostream & os) const { generate_vtable<true>(os); }
	void generate_vtable_definition_paramters(std::ostream & os) const {
		auto first{true};
		for(const auto & p : params) {
			if(first) first = false;
			else os << ", ";
			os << "*" << p.name;
		}
	}

	auto empty() const noexcept -> bool { return params.empty(); }
};

class comment_list final {
	std::vector<std::string> comments;
public:
	comment_list() =default;
	comment_list(parser & p) { while(p.starts_with('/')) comments.emplace_back(p.next()); }

	void generate(std::ostream & os) const { for(const auto & c : comments) os << c << '\n'; }
};

class constructor final {
	comment_list clist;
	param_list plist;
public:
	constructor() =default;
	constructor(parser & p, comment_list clist) : clist{std::move(clist)}, plist{p} {}

	void generate_definition(std::ostream & os, std::string_view class_, std::size_t no) const {
		clist.generate(os);
		os << class_ << "(";
		plist.generate_param_list(os);
		os << ") { cwc_dll().call<&cwc_vtable::cwc_" << no << ">(";
		plist.generate_param_passing(os);
		if(!plist.empty()) os << ", ";
		os << "&cwc_self";
		os << "); }\n";
	}

	void generate_vtable_declaration(std::ostream & os, std::size_t no) const {
		os << "cwc::internal::error_callback(CWC_CALL * cwc_" << no << ")(";
		plist.generate_vtable_declaration(os);
		if(!plist.empty()) os << ", ";
		os << "cwc_impl **) noexcept;\n";
	}

	void generate_vtable_definition(std::ostream & os, std::string_view fqn, bool last) const {
		os << "+[](";
		plist.generate_vtable_definition(os);
		if(!plist.empty()) os << ", ";
		os << fqn << "::cwc_impl ** cwc_result";
		os << ") noexcept { return cwc::internal::error_marshalling([&] { *cwc_result = new " << fqn << "::cwc_impl{";
		plist.generate_vtable_definition_paramters(os);
		os << "}; }); }";
		if(!last) os << ",";
		os << "\\\n";
	}
};

class method final {
	comment_list clist;
	std::string name;
	param_list plist;
	bool const_;
	std::optional<std::string> result;
public:
	method(parser & p, comment_list clist) : clist{std::move(clist)} {
		const auto returning{p.consume("auto")};
		if(!returning) p.expect("void");

		name = p.next();
		plist = p;
		const_ = p.consume("const");
		if(returning) {
			p.expect("->");
			result = p.next();
		}
	}

	void generate_definition(std::ostream & os, std::size_t no) const {
		clist.generate(os);
		os << (result ? "auto" : "void") << " " << name << "(";
		plist.generate_param_list(os);
		os << ") ";
		if(const_) os << "const ";
		if(result) os << "-> " << *result << " ";
		os << "{ ";
		if(result) os << "\n" << *result << " cwc_result;\n";
		os << "cwc_dll().call<&cwc_vtable::cwc_" << no << ">(cwc_self";
		if(!plist.empty() || result) os << ", ";
		plist.generate_param_passing(os);
		if(result) {
			if(!plist.empty()) os << ", ";
			os << "std::addressof(cwc_result)";
		}
		os << ");";
		if(result) os << "\nreturn cwc_result;\n";
		else os << " ";
		os << "}\n";
	}

	void generate_vtable_declaration(std::ostream & os, std::size_t no) const {
		os << "cwc::internal::error_callback(CWC_CALL * cwc_" << no << ")(";
		if(const_) os << "const ";
		os << "cwc_impl *";
		if(!plist.empty() || result) os << ", ";
		plist.generate_vtable_declaration(os);
		if(result) {
			if(!plist.empty()) os << ", ";
			os << *result << " *";
		}
		os << ") noexcept;\n";
	}

	void generate_vtable_definition(std::ostream & os, std::string_view fqn, bool last) const {
		os << "+[](";
		if(const_) os << "const ";
		os << fqn << "::cwc_impl * cwc_self";
		if(!plist.empty() || result) os << ", ";
		plist.generate_vtable_definition(os);
		if(result) {
			if(!plist.empty()) os << ", ";
			os << *result << " * cwc_result";
		}
		os << ") noexcept { return cwc::internal::error_marshalling([&] { ";
		if(result) os << "*cwc_result = ";
		os << "cwc_self->" << name << "(";
		plist.generate_vtable_definition_paramters(os);
		os << "); }); }";
		if(!last) os << ",";
		os << "\\\n";
	}
};

class component final {
	comment_list clist;
	std::string dll, name;
	std::vector<constructor> constructors;
	std::vector<method> methods;
public:
	component(parser & p) : clist{p} {
		p.expect("[[");
		p.expect("library");
		p.expect("=");
		dll = p.next();
		p.expect("]]");
		p.expect("component");
		name = p.next();
		p.expect("{");
		while(!p.consume("}")) {
			comment_list comments{p};
			if(p.consume(name)) constructors.emplace_back(p, std::move(comments));
			else methods.emplace_back(p, std::move(comments));
			p.expect(";");
		}
		p.expect(";");

		if(constructors.empty()) constructors.emplace_back();
	}

	void generate(std::ostream & os, const std::string & namespace_) const {
		const auto fqn{namespace_ + "::" + name};
		const auto mangled_name{[&] {
			std::string mangled_name;
			std::stringstream ss{namespace_};
			for(char c; ss >> c;) {
				if(c == ':') {
					ss >> c;
					mangled_name += '$';
				} else mangled_name += c;
			}
			mangled_name += '$';
			mangled_name += name;
			return mangled_name;
		}()};
		clist.generate(os);
		os << "struct " << name << " {\n";
		os << "struct cwc_impl;\n";
		os << "struct cwc_vtable final {\n";
		os << "void (CWC_CALL * cwc_0)(cwc_impl *) noexcept;\n";
		{
			std::size_t no{1};
			for(const auto & c : constructors) c.generate_vtable_declaration(os, no++);
			for(const auto & m : methods) m.generate_vtable_declaration(os, no++);
		}
		os << "};\n";
		os << "\n";
		os << name << "(const " << name << " &) =delete;\n";
		os << name << "(" << name << " && cwc_other) noexcept : cwc_self{std::exchange(cwc_other.cwc_self, nullptr)} {}\n";
		os << "auto operator=(const " << name << " &) -> " << name << " & =delete;\n";
		os << "auto operator=(" << name << " && cwc_other) noexcept -> " << name << " & { std::swap(cwc_self, cwc_other.cwc_self); return *this; }\n";
		os << "~" << name << "() noexcept { cwc_dll().call<&cwc_vtable::cwc_0>(cwc_self); }\n";
		os << "\n";
		os << "\n";
		{
			std::size_t no{1};
			for(const auto & c : constructors) c.generate_definition(os, name, no++);
			if(!methods.empty()) os << "\n";
			for(const auto & m : methods) m.generate_definition(os, no++);
		}
		os << "private:\n";
		os << "static\n";
		os << "auto cwc_dll() -> const cwc::internal::dll & {\n";
		os << "static const cwc::internal::dll instance{\"" << dll << "\", \"" << mangled_name << "\"};\n";
		os << "return instance;\n";
		os << "}\n";
		os << "\n";
		os << "cwc_impl * cwc_self;\n";
		os << "};\n";
		os << "#define CWC_EXPORT_" << mangled_name << " \\\n";
		os << "extern \"C\" CWC_EXPORT const " << fqn << "::cwc_vtable " << mangled_name << "{\\\n";
		os << "+[](" << fqn << "::cwc_impl * cwc_self) noexcept { delete cwc_self; },\\\n";
		{
			const auto count{constructors.size() + methods.size()};
			std::size_t no{0};
			for(const auto & c : constructors) c.generate_vtable_definition(os, fqn, ++no == count);
			for(const auto & m : methods) m.generate_vtable_definition(os, fqn, ++no == count);
		}
		os << "}\n";
	}
};

class namespace_ final {
	comment_list clist;
	std::string name;
	std::vector<component> components;
public:
	namespace_(parser & p) : clist{p} {
		p.expect("namespace");
		name = p.next();
		p.expect("{");
		while(!p.consume("}")) components.emplace_back(p);
	}

	void generate(std::ostream & os) const {
		clist.generate(os);
		os << "namespace " << name << " {\n";
		auto first{true};
		for(const auto & c : components) {
			if(first) first = false;
			else os << "\n\n";
			c.generate(os, name);
		}
		os << "}\n";
	}
};

class cwcc final {
	std::vector<namespace_> namespaces;
public:
	cwcc(parser & p) { while(p.has_tokens()) namespaces.emplace_back(p); }

	void generate(std::ostream & os) const {
		os << "#pragma once\n";
		os << "#include <cwc/cwc.hpp>\n\n";

		//TODO: some kind of disclaimer
		auto first{true};
		for(const auto & n : namespaces) {
			if(first) first = false;
			else os << "\n\n";
			n.generate(os);
		}
	}
};


void indent(std::istream & is, std::ostream & os) {
	std::size_t indent{0};

	for(std::string line; std::getline(is, line);) {
		if(line.empty()) {
			os << "\n";
			continue;
		}

		if(line.front() == ')' || line.front() == '}' || line == "private:") std::fill_n(std::ostream_iterator<char>{os}, indent - 1, '\t');
		else std::fill_n(std::ostream_iterator<char>{os}, indent, '\t');
		os << line << '\n';

		for(const char & c : line) {
			switch(c) {
				case '(':
				case '{':
					++indent;
					break;
				case ')':
				case '}':
					--indent;
					break;
				case '/': goto done;
			}
		}
done:
		(void)0;
	}
}


int main(int argc, char * argv[]) {
	std::stringstream ss{R"(
		namespace cwc::sample::fibonacci {
			[[library=sample-fibonacci]]
			component sequence {
				sequence();
				sequence(const int & dummy);

				auto calculate(
					const std::uint8_t & no //!< no
				) const -> std::uint64_t;
			};

			[[library=sample]]
			component seq {

			};
		}

		namespace sa {
			[[library=say]]
			component s {

			};
		}
	)"};
	try {
		parser p{ss};
		cwcc c{p};

		std::stringstream ss;
		c.generate(ss);
		indent(ss, std::cout);
	} catch(const std::exception & exc) { std::cerr << "ERROR: " << exc.what() << std::endl; }

}