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
NAMESPACE       =      COMMENT* "namespace" NAMESPACE_NAME '{' COMPONENT* '}'
COMPONENT       =      COMMENT* ATTRIBUTES "component" NAME '{' BODY* '}' ';'
BODY            =      CONSTRUCTOR | METHOD
CONSTRUCTOR     =      COMMENT* NAME ARG_LIST
METHOD          =      COMMENT* ("void" | "auto") NAME ARG_LIST CONST "->" TYPE ';'
ARG_LIST        =      '(' ARGS ')'
ARGS            =      ARG (',' ARG)*
ARG             =      ("const" TYPE '&' NAME | TYPE REF NAME) COMMENT
REF             =      "&"?
CONST           =      "const"?
COMMENT         =      "//" .* \n
ATTRIBUTES      =      "[[" "library" '=' LIB_NAME "]]"
//TODO: TYPE can be template - grammar must include <()> ...
TYPE            =      NAME
NAME            =      (a-zA-Z)(a-zA-Z0-9_)*
NAMESPACE_NAME  =      NAME("::"NAME)*
//TODO: LIB_NAME is not correctly implemented in parser
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

	auto next() -> std::string {
		std::string result;
		result = std::move(tokens.front());
		tokens.pop();
		return result;
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
							} else if(c == '<') {
								auto nested{1};
								token += '<';
								while(nested) {
									in >> c;
									switch(c) {
										case '<': ++nested; break;
										case '>': --nested; break;
									}
									token += c;
								}
							} else {
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


	auto name() -> std::string {
		auto tmp{next()};
		if(tmp.find(':') != tmp.npos) throw std::logic_error{"expected name - found namespace_name"};
		if(tmp.find('<') != tmp.npos) throw std::logic_error{"expected name - found template_name"};
		if(tmp.find('-') != tmp.npos) throw std::logic_error{"expected name - found lib_name"};
		return tmp;
	}

	auto namespace_name() -> std::string {
		auto tmp{next()};
		if(tmp.find('<') != tmp.npos) throw std::logic_error{"expected nested_name - found template_name"};
		if(tmp.find('-') != tmp.npos) throw std::logic_error{"expected nested_name - found lib_name"};
		return tmp;
	}

	auto comment() -> std::string {
		auto tmp{next()};
		if(tmp.front() != '/') throw std::logic_error{"expected comment - found something else"};
		return tmp;
	}

	auto dll_name() -> std::string {
		auto tmp{next()};
		//TODO: validation
		if(tmp.find('<') != tmp.npos) throw std::logic_error{"expected lib_name - found template_name"};
		if(tmp.find('/') != tmp.npos) throw std::logic_error{"expected lib_name - found comment"};
		return tmp;
	}

	auto type() -> std::string { return next(); }

	auto starts_with(char ch) const -> bool {
		if(tokens.empty()) return false;
		return tokens.front()[0] == ch;
	}

	auto has_tokens() const noexcept -> bool { return !tokens.empty(); }
};


class param_list final {
	struct param final {
		bool const_;
		bool ref;
		std::string type, name, trailing_comment;

		param(parser & p) {
			const_ = p.consume("const");
			type = p.type();
			if(const_) {
				ref = true;
				p.expect("&");
			} else ref = p.accept("&");
			name = p.name();
			if(p.starts_with('/')) trailing_comment = p.comment();
		}

		void generate_param(std::ostream & os) const {
			if(const_) os << "const ";
			os << type << " ";
			if(ref) os << "& ";
			os << name;
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
			os << p.type;
			if(p.ref) os << " *";
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
			if(p.ref) os << "std::addressof(";
			else os << "std::move(";
			os << p.name << ")";
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
			os << p.type << " ";
			if(p.ref) os << "& ";
			os << p.name;
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
			if(p.ref) os << "*";
			else os << "std::move(";
			os << p.name;
			if(!p.ref) os << ")";
		}
	}

	auto empty() const noexcept -> bool { return params.empty(); }
};

class comment_list final {
	std::vector<std::string> comments;
public:
	comment_list() =default;
	comment_list(parser & p) { while(p.starts_with('/')) comments.emplace_back(p.comment()); }

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
		os << ") noexcept { return cwc::internal::try_([&] { *cwc_result = new " << fqn << "::cwc_impl{";
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

		name = p.name();
		plist = p;
		const_ = p.consume("const");
		if(returning) {
			p.expect("->");
			result = p.type();
		}
	}

	void generate_definition(std::ostream & os, std::size_t no) const {
		clist.generate(os);
		os << (result ? "auto" : "void") << " " << name << "(";
		plist.generate_param_list(os);
		os << ") ";
		if(const_) os << "const ";
		if(result) os << "-> " << *result << " ";
		os << "{";
		if(result) os << "\n" << *result << " cwc_result;\n";
		else os << " ";
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
		os << ") noexcept { return cwc::internal::try_([&] { ";
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
		dll = p.dll_name();
		p.expect("]]");
		p.expect("component");
		name = p.name();
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
		name = p.namespace_name();
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
};


void indent(std::istream & is, std::ostream & os) {
	std::size_t indent{0};

	for(std::string line; std::getline(is, line);) {
		if(line.empty()) {
			os << "\n";
			continue;
		}

		if(line.front() == '#');
		else if(line.front() == ')' || line.front() == '}' || line == "private:") std::fill_n(std::ostream_iterator<char>{os}, indent - 1, '\t');
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
					std::uint8_t no //!< no
				) const -> std::uint64_t;
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