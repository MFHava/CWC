
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "nodes.hpp"

namespace cwcc {
	auto operator<<(std::ostream & os, const enum_::member & self) -> std::ostream & {
		for(const auto & doc : self.lines) os << "\t\t" << doc << '\n';
		return os << "\t\t" << self.name << ',';
	}

	auto operator<<(std::ostream & os, const enum_ & self) -> std::ostream & {
		if(self.members.size() > self.max_size) throw std::length_error{"ERROR: enums only support up to " + std::to_string(self.max_size) + " members"};
		if(self.members.empty()) throw std::logic_error{"ERROR: empty enums are not supported"};
		for(const auto & doc : self.lines) os << '\t' << doc << '\n';
		os << "\tenum class " << self.name << " : ::cwc::uint8 {\n";
		for(const auto & e : self.members) os << e << '\n';
		return os << "\t};";
	}

	auto operator<<(std::ostream & os, const struct_::member & self) -> std::ostream & {
		os << self.type << ' ' << self.fields[0];
		for(std::size_t i{1}; i < self.fields.size(); ++i) os << ", " << self.fields[i];
		return os << ';';
	}

	auto operator<<(std::ostream & os, const struct_ & self) -> std::ostream & {
		os << "\tCWC_PACK_BEGIN\n";
		for(const auto & doc : self.lines) os << '\t' << doc << '\n';
		os << "\tstruct " << self.name << " {\n";
		for(auto & member : self.members) {
			for(const auto & doc : member.lines) os << "\t\t" << doc << '\n';
			os << "\t\t" << member << '\n';
		}
		for(const auto & member : self.members) os << "\t\tstatic_assert(std::is_standard_layout<" << member.type << ">::value, \"Type '" << member.type << "' is incompatible with the requirements of portable structs\");\n";
		return os << "\t};\n"
		             "\tCWC_PACK_END";
	}

	auto operator<<(std::ostream & os, const typedef_ & self) -> std::ostream & {
		for(const auto & doc : self.lines) os << '\t' << doc << '\n';
		return os << "\tusing " << self.name << " = " << self.type << ";\n"
		             "\tstatic_assert(std::is_standard_layout<" << self.type << ">::value, \"Type '" << self.type << "' is incompatible with the requirements for portable type definitions\");";//TODO: is it safe to generate this every time?!
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