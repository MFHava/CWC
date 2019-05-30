
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "nodes.hpp"
#include "indent.hpp"

namespace cwcc {
	auto operator<<(std::ostream & os, const enum_::member & self) -> std::ostream & {
		indent_scope scope{os};
		for(const auto & doc : self.lines) os << indent << doc << '\n';
		return os << indent << self.name << ',';
	}

	auto operator<<(std::ostream & os, const enum_ & self) -> std::ostream & {
		if(self.members.size() > self.max_size) throw std::length_error{"ERROR: enums only support up to " + std::to_string(self.max_size) + " members"};
		if(self.members.empty()) throw std::logic_error{"ERROR: empty enums are not supported"};
		for(const auto & doc : self.lines) os << indent << doc << '\n';
		os << indent << "enum class " << self.name << " : ::cwc::uint8 {\n";
		for(const auto & e : self.members) os << e << '\n';
		os << indent << "};\n"
		   << indent << "inline\n"
		   << indent << "auto operator<<(std::ostream & os, const " << self.name << " & self) -> std::ostream & {\n";
		{
			indent_scope scope{os};
			os << indent << "switch(self) {\n";
			{
				indent_scope scope{os};//TODO: can be merged with loop in C++20
				for(const auto & e : self.members) os << indent << "case " << self.name << "::" << e.name << ": return os << \"" << self.name << "::" << e.name << "\";\n";
				os << indent << "default: return os << \"invalid value for enum \\\"" << self.name << "\\\"\";\n";
			}
			os << indent << "}\n";
		}
		os << indent << "}\n";
		return os;
	}

	auto operator<<(std::ostream & os, const struct_::member & self) -> std::ostream & {
		os << self.type << ' ' << self.fields[0];
		for(std::size_t i{1}; i < self.fields.size(); ++i) os << ", " << self.fields[i];
		return os << ';';
	}

	auto operator<<(std::ostream & os, const struct_ & self) -> std::ostream & {
		os << indent << "CWC_PACK_BEGIN\n";
		for(const auto & doc : self.lines) os << indent << doc << '\n';
		os << indent << "struct " << self.name << " {\n";
		{
			indent_scope scope{os};
			for(auto & member : self.members) {
				for(const auto & doc : member.lines) os << indent << doc << '\n';
				os << indent << member << '\n';
			}
			for(const auto & member : self.members) os << indent << "static_assert(std::is_standard_layout_v<" << member.type << ">);\n";
		}
		return os << indent << "};\n"
		          << indent << "CWC_PACK_END";
	}

	auto operator<<(std::ostream & os, const typedef_ & self) -> std::ostream & {
		for(const auto & doc : self.lines) os << indent << doc << '\n';
		return os << indent << "using " << self.name << " = " << self.type << ";\n"
		          << indent << "static_assert(std::is_standard_layout_v<" << self.type << ">);";//TODO: is it safe to generate this every time?!
	}

	auto component::factory() const -> interface {
		interface p;
		p.name = "cwc_factory";
		intrusive_ptr ret;
		ret.mutable_ = true;
		ret.type = untemplated_type{"::cwc::component"};

		auto transform = [&](const constructor & c) {
			interface::method m;
			m.lines = c.lines;
			m.name = "create";
			m.in = c.params;
			m.out = ret;
			p.methods.push_back(m);
		};

		if(members.empty()) transform(constructor());
		else for(const auto & member : members)
			transform(member);

		return p;
	}
}