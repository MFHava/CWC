
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <stdexcept>
#include "parser.hpp"

//TODO: move grammar to doxygen files
/* GRAMMAR

CWC                  =      NAMESPACE*;
NAMESPACE            =      COMMENT* "namespace" NAMESPACE_NAME '{' COMPONENT* '}'
COMPONENT            =      COMMENT* ATTRIBUTES "component" NAME '{' BODY* '}' ';'
BODY                 =      (CONSTRUCTOR | METHOD | STATIC_METHOD) ';'
CONSTRUCTOR          =      COMMENT* NAME ARG_LIST
STATIC_METHOD        =      COMMENT* "static" (STATIC_AUTO_METHOD | STATIC_VOID_METHOD)
STATIC_VOID_METHOD   =      "void" NAME ARG_LIST NOEXCEPT
STATIC_AUTO_METHOD   =      "auto" NAME ARG_LIST NOEXCEPT "->" TYPE
METHOD               =      COMMENT* (AUTO_METHOD | VOID_METHOD)
AUTO_METHOD          =      "auto" NAME ARG_LIST CONST NOEXCEPT "->" TYPE
VOID_METHOD          =      "void" NAME ARG_LIST CONST NOEXCEPT
ARG_LIST             =      '(' ARGS ')'
ARGS                 =      (ARG (',' ARG)*)?
ARG                  =      ("const" TYPE "&" NAME | TYPE REF NAME) COMMENT
REF                  =      ("&" | "&&")?
CONST                =      "const"?
NOEXCEPT             =      "noexcept"?
COMMENT              =      "//" .* \n
ATTRIBUTES           =      "[[" "library" '=' LIB_NAME "]]"
TYPE                 =      NAME TEMPLATE?
TEMPLATE             =      '<' (TEMPLATE | [^>])* '>'
NAME                 =      (a-zA-Z)(a-zA-Z0-9_)*
NAMESPACE_NAME       =      NAME("::"NAME)*
LIB_NAME             =      (a-zA-Z0-9_-)+
*/

namespace cwcc {
	namespace {
		void throw_stray(char c) {
			std::string msg{"detected stray '"};
			msg += c;
			msg += '\'';
			throw std::logic_error{msg};
		}
	}

	auto parser::next() -> std::string {
		std::string result;
		result = std::move(tokens.front());
		tokens.pop();
		return result;
	}

	parser::parser(std::istream & in) {
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
				case '&': {
					in >> c;
					if(c == '&') tokens.push("&&");
					else {
						tokens.push("&");
						goto retry;
					}
				} break;
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
							if(c == '_' || c == '-' || std::isalnum(c)) token += c;
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

	void parser::expect(std::string_view str) {
		if(accept(str)) tokens.pop();
		else {
			std::ostringstream os;
			os << "expected \"" << str << "\" - ";
			if(tokens.empty()) os << "EOF reached";
			else os << "encountered \"" << tokens.front() << "\"";
			throw std::invalid_argument{os.str()};
		}
	}

	auto parser::accept(std::string_view str) const -> bool {
		if(tokens.empty()) return false;
		return tokens.front() == str;
	}

	auto parser::consume(std::string_view str) -> bool {
		if(accept(str)) {
			tokens.pop();
			return true;
		} else return false;
	}

	auto parser::next(type type) -> std::string {
		auto tmp{next()};
		switch(type) {
			case type::name:
				if(tmp.find(':') != tmp.npos) throw std::logic_error{"expected name - found namespace_name"};
				if(tmp.find('<') != tmp.npos) throw std::logic_error{"expected name - found template_name"};
				if(tmp.find('-') != tmp.npos) throw std::logic_error{"expected name - found lib_name"};
				break;
			case type::namespace_:
				if(tmp.find('<') != tmp.npos) throw std::logic_error{"expected nested_name - found template_name"};
				if(tmp.find('-') != tmp.npos) throw std::logic_error{"expected nested_name - found lib_name"};
				break;
			case type::comment:
				if(tmp.front() != '/') throw std::logic_error{"expected comment - found something else"};
				break;
			case type::library:
				if(tmp.find('<') != tmp.npos) throw std::logic_error{"expected lib_name - found template_name"};
				if(tmp.find('/') != tmp.npos) throw std::logic_error{"expected lib_name - found comment"};
				break;
			case type::type:
				break;
			default:
				std::abort(); //unreachable
				break;
		}
		return tmp;
	}

	auto parser::starts_with(char ch) const -> bool {
		if(tokens.empty()) return false;
		return tokens.front()[0] == ch;
	}
}
