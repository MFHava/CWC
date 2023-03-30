
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <cassert>
#include <iterator>
#include <stdexcept>
#include <algorithm>
#include "parser.hpp"

//TODO: move grammar to doxygen files
/* CWC GRAMMAR

CWCC ::= INCLUDE* (COMMENT | NAMESPACE)*
INCLUDE ::= '#' 'include' (STRING | SYS_STRING)
COMMENT ::= '//' .* EOF
NAMESPACE ::= namespace NS_IDENT '{' (COMMENT | LIBRARY | TEMPLATE)* '}'
LIBRARY ::= '@library' '(' STRING ')' (EXTERN | COMPONENT)
EXTERN ::= 'extern' 'template' 'component' IDENT '<' TPARAM % ',' '>' ';'
TEMPLATE ::= 'template' '<' (TYPE IDENT ) % ',' '>' COMPONENT
COMPONENT ::= VERSION 'component' ATTRIBUTE* IDENT ['final'] '{' (ATTRIBUTE | COMMENT | CONSTRUCTOR | METHOD | USING)* '}' ';'
CONSTRUCTOR ::= ['explicit'] IDENT '(' PARAM % ',' ')' ['=' 'delete'] ';'
METHOD ::= ['static' ('auto' | 'void') ('operator' '(' ')' | IDENT) '(' PARAM % ',' ')' ['const'] [('&' | '&&')] ['noexcept'] ['->' TYPE] ['=' 'delete'] ';'
PARAM ::= ((const TYPE '&') | ('const' TYPE ('&' | '&&'))) IDENT
USING ::= 'using' IDENT '=' TYPE ';'
ATTRIBUTE ::= '[[' IDENT ['(' STRING ')'] ']]'
TYPE ::= NS_IDENT ['<' ?* '>']
TPARAM ::= SIGNED_NUMBER | TYPE
SIGNED_NUMBER ::= ['+' | '-'] NUMBER
NUMBER ::= 0 | (1-9)(0-9)*
NS_IDENT ::= [NS_IDENT '::'] IDENT
IDENT ::= (a-zA-Z)(a-zA-Z0-9_)*
STRING ::= '"' [^"]* '"'
SYS_STRING ::= '<' [^>]* '>'
VERSION ::= '@version' '(' NUMBER ')'
*/

namespace cwcc {
	using namespace std::string_literals;

	void parser::skip_ws() noexcept { pos = std::find_if_not(pos, std::cend(content), [](char c) { return std::isspace(c); }); }

	auto parser::expect_delimited(char first, char last) -> std::string_view { //TODO: does not support escaping...
		if(!*this) throw std::invalid_argument{"EOF when expecting delimited"};
		if(*pos != first) throw std::invalid_argument{"expected_delimited does not start with '"s + first + "'"};

		auto it{std::find(pos + 1, std::cend(content), last)};
		if(it == std::cend(content)) throw std::invalid_argument{"expected_delimited does not end with '"s + last + "'"};

		++it;
		std::string_view result{&*pos, static_cast<std::size_t>(it - pos)};
		pos = it;
		skip_ws();
		return result;
	}

	auto parser::accept(std::string_view str) const noexcept -> bool {
		const auto [sit, cit]{std::mismatch(std::cbegin(str), std::cend(str), pos, std::cend(content))};
		return sit == std::cend(str);
	}

	auto parser::consume(std::string_view str) noexcept -> bool {
		assert(!str.empty());

		const auto [sit, cit]{std::mismatch(std::cbegin(str), std::cend(str), pos, std::cend(content))};
		if(sit != std::cend(str)) return false;

		pos = cit;
		skip_ws();
		return true;
	}

	void parser::expect(std::string_view str) {
		assert(!str.empty());

		const auto [sit, cit]{std::mismatch(std::cbegin(str), std::cend(str), pos, std::cend(content))};
		if(sit != std::cend(str)) throw std::invalid_argument{"expected(" + std::string{str} +") failed"};

		pos = cit;
		skip_ws();
	}

	auto parser::expect_namespace() -> std::string_view {
		if(!*this) throw std::invalid_argument{"EOF when namespace_id was required"};

		auto it{pos};
		do {
			if(!std::isalpha(*it)) throw std::invalid_argument{"invalid start of namespace_id segment"};
			for(++it; it != std::cend(content) && (*it == '_' || std::isalnum(*it)); ++it);
			if(it == std::cend(content) || *it != ':') break;
			++it;
			if(it == std::cend(content) || *it != ':') throw std::invalid_argument{"found trailing ':' when parsing namespace_id"};
			++it;
		} while(true);
		assert(it != pos);

		std::string_view result{&*pos, static_cast<std::size_t>(it - pos)};

		pos = it;
		skip_ws();
		return result;
	}

	auto parser::expect_comment() -> std::string_view {
		if(!*this) throw std::invalid_argument{"EOF when comment was required"};
		if(*pos != '/') throw std::invalid_argument{"invalid start of comment"};

		auto it{pos + 1};
		if(it == std::cend(content) || *it != '/') throw std::invalid_argument{"found trailing '/' when parsing comment"};
		it = std::find(it + 1, std::cend(content), '\n');

		if(it != std::cend(content)) ++it;
		std::string_view result{&*pos, static_cast<std::size_t>(it - pos)};

		pos = it;
		skip_ws();
		return result;
	}
	
	auto parser::expect_string_literal() -> std::string_view { return expect_delimited('"', '"'); }

	auto parser::expect_system_header() -> std::string_view { return expect_delimited('<', '>'); }

	auto parser::expect_name() -> std::string_view {
		if(!*this) throw std::invalid_argument{"EOF when name was required"};
		if(!std::isalpha(*pos)) throw std::invalid_argument{"invalid start of name"};

		const auto it{std::find_if_not(pos + 1, std::cend(content) ,[](char ch) { return std::isalnum(ch) || ch == '_'; })};

		std::string_view result{&*pos, static_cast<std::size_t>(it - pos)};
		assert(!std::isspace(result.back()));

		pos = it;
		skip_ws();
		return result;
	}

	auto parser::expect_type() -> std::string_view {
		if(!*this) throw std::invalid_argument{"EOF when type was required"};

		auto it{pos};
		std::size_t nesting{0};
		do {
			if(it == std::cend(content)) break;
			if(!nesting) {
				if(std::isspace(*it)) goto done;
				if(!std::isalpha(*it)) throw std::invalid_argument{"invalid start of type segment"};
			}
			for(; it != std::cend(content) && (*it == '_' || std::isalnum(*it)); ++it);
			if(it == std::cend(content)) break;
retry:
			switch(*it) {
				case ':': {
					++it;
					if(it == std::cend(content) || *it != ':') throw std::invalid_argument{"found trailing ':' when parsing type"};
					++it;
					if(it == std::cend(content)) throw std::invalid_argument{"found trailing \"::\" when parsing type"};
				} break;
				case ',': {
					if(!nesting) goto done;
					for(++it; it != std::cend(content) && std::isspace(*it); ++it);
				} break;
				case '<': ++nesting; ++it; break;
				case '>':
					if(!nesting) goto done;
					--nesting;
					++it;
					if(nesting) for(; it != std::cend(content) && std::isspace(*it); ++it);
					if(it == std::cend(content)) goto done;
					if(*it == '>' || *it == ':' || *it == ',' || *it == ':' || *it == ';') goto retry; //TODO: this is essentially a hack
					break;
				default:
					if(!nesting) goto done;
					++it;
					break;
			}
		} while(true);
done:
		assert(it != pos);
		if(nesting) throw std::invalid_argument{"could not extract type, had unbalanced <>"};
		std::string_view result{&*pos, static_cast<std::size_t>(it - pos)};
		while(std::isspace(result.back())) result.remove_suffix(1);
		pos = it;
		skip_ws();
		return result;
	}

	auto parser::expect_tparam() -> std::string_view {
		if(!*this) throw std::invalid_argument{"EOF when tparam was required"};
		return std::isalpha(*pos) ? expect_type() : expect_number();
	}

	auto parser::expect_number() -> std::string_view {
		if(!*this) throw std::invalid_argument{"EOF when number was required"};

		auto it{pos};
		if(*it == '+' || *it == '-') ++it;
		if(!std::isdigit(*it)) throw std::invalid_argument{"invalid start of number"};

		for(; it != std::cend(content) && std::isdigit(*it); ++it);
		if(*pos == '0' && it != pos + 1) throw std::invalid_argument{"octal/hexadecimal numbers are not supported"};

		std::string_view result{&*pos, static_cast<std::size_t>(it - pos)};
		pos = it;
		skip_ws();
		return result;
	}

	auto parser::expect_version() -> std::string_view {
		if(!*this) throw std::invalid_argument{"EOF when version was required"};
		if(!std::isdigit(*pos)) throw std::invalid_argument{"invalid start of version"};

		auto it{pos};
		if(*it == '0') {
			++it;
			if(it != std::cend(content) && std::isdigit(*it)) throw std::invalid_argument{"only decimal version numbers are supported"};
		} else for(++it; it != std::cend(content) && std::isdigit(*it); ++it);

		std::string_view result{&*pos, static_cast<std::size_t>(it - pos)};
		pos = it;
		skip_ws();
		return result;
	}
}
