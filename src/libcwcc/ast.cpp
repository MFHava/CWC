
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "ast.hpp"
#include "parser.hpp"

namespace cwcc {
	void attribute::parse(parser & p) {
		p.expect("[[");
		name = p.expect_name();
		if(p.consume("(")) {
			reason = p.expect_string_literal();
			p.expect(")");
		}
		p.expect("]]");
	}

	void param::parse(parser & p) {
		const_ = p.consume("const");
		type = p.expect_type();
		if(const_) {
			p.expect("&");
			ref = ref_t::lvalue;
		} else {
			if(p.consume("&&")) ref = ref_t::rvalue;
			else if(p.consume("&")) ref = ref_t::lvalue;
		}
		name = p.expect_name();
	}

	void comment::parse(parser & p) { line = p.expect_comment(); }

	void constructor::parse(parser & p) {
		name = p.expect_name();
		p.expect("(");
		if(!p.consume(")")) {
			do {
				param p_;
				p_.parse(p);
				params.emplace_back(std::move(p_));
			} while(p.consume(","));
			p.expect(")");
		}
		if(delete_ = p.consume("=")) p.expect("delete");
		p.expect(";");
	}

	void method::parse(parser & p) {
		static_ = p.consume("static");
		const auto returning{p.consume("auto")};
		if(!returning) p.expect("void");
		if(!static_ && p.consume("operator")) { //TODO: [C++23] adds support for static operator()
			p.expect("(");
			p.expect(")");
			name = "operator()";
		} else name = p.expect_name();
		p.expect("(");
		if(!p.consume(")")) {
			do {
				param p_;
				p_.parse(p);
				params.emplace_back(std::move(p_));
			} while(p.consume(","));
			p.expect(")");
		}
		if(!static_) {
			const_ = p.consume("const");
			if(p.consume("&&")) ref = ref_t::rvalue;
			else if(p.consume("&")) ref = ref_t::lvalue;
		}
		noexcept_ = p.consume("noexcept");
		if(returning) {
			p.expect("->");
			result = p.expect_type();
		}
		if(delete_ = p.consume("=")) p.expect("delete");
		p.expect(";");
	}

	void using_::parse(parser & p) {
		p.expect("using");
		name = p.expect_name();
		p.expect("=");
		type = p.expect_type();
		p.expect(";");
	}

	void component::parse(parser & p) {
		p.expect("component");
		while(p.accept("[")) attributes.emplace_back().parse(p);
		name = p.expect_name();
		final = p.consume("final");
		p.expect("{");

		std::size_t no{0};
		while(!p.consume("}")) {
			if(p.accept("[")) {
				attribute a;
				a.parse(p);
				content.emplace_back(std::move(a));
			} else if(p.accept("/")) {
				comment c;
				c.parse(p);
				content.emplace_back(std::move(c));
			} else if(p.accept("using")) {
				using_ u;
				u.parse(p);
				content.emplace_back(std::move(u));
			} else if(p.accept(name)) {
				constructor c;
				c.parse(p);
				content.emplace_back(std::move(c));
			} else {
				method m;
				m.parse(p);
				content.emplace_back(std::move(m));
			}
		}
		p.expect(";");
	}

	void template_::parse(parser & p) {
		p.expect("template");
		p.expect("<");
		do {
			p.expect("typename");
			tparams.emplace_back(p.expect_name());
		} while(p.consume(","));
		p.expect(">");
		component_.parse(p);
	}

	void extern_::parse(parser & p) {
		p.expect("extern");
		p.expect("template");
		p.expect("component");
		component = p.expect_name();
		p.expect("<");
		do {
			tparams.emplace_back(p.expect_type()); //TODO: NTTPs are currently not supported...
		} while(p.consume(","));
		p.expect(">");
		p.expect(";");
	}

	void library::parse(parser & p) {
		p.expect("@library");
		p.expect("(");
		name = p.expect_string_literal();
		p.expect(")");
		if(p.accept("extern")) {
			extern_ e;
			e.parse(p);
			content = std::move(e);
		} else {
			component c;
			c.parse(p);
			content = std::move(c);
		}
	}

	void namespace_::parse(parser & p) {
		p.expect("namespace");
		name = p.expect_namespace();
		p.expect("{");
		while(!p.consume("}")) {
			if(p.accept("/")) {
				comment c;
				c.parse(p);
				content.emplace_back(std::move(c));
			} else if(p.accept("@")) {
				library l;
				l.parse(p);
				content.emplace_back(std::move(l));
			} else {
				template_ t;
				t.parse(p);
				content.emplace_back(std::move(t));
			}
		}
	}

	void include::parse(parser & p) {
		p.expect("#");
		p.expect("include");
		if(p.accept("\"")) header = p.expect_string_literal();
		else header = p.expect_system_header();
	}

	void cwc::parse(parser & p) { //TODO: [C++20] support for header units?
		while(p) {
			if(p.accept("#")) {
				include i;
				i.parse(p);
				content.emplace_back(std::move(i));
			} else if(p.accept("/")) {
				comment c;
				c.parse(p);
				content.emplace_back(std::move(c));
			} else {
				namespace_ n;
				n.parse(p);
				content.emplace_back(std::move(n));
			}
		}
	}
}
