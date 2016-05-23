
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include "line.hpp"
#include "nodes.hpp"

namespace cwcc {
	auto operator<<(std::ostream & out, const enum_::member & self) -> std::ostream & {
		for(const auto & doc : self.lines) out << "\t\t" << doc << '\n';
		out << "\t\t" << self.name;
		return out;
	}

	auto operator<<(std::ostream & out, const enum_ & self) -> std::ostream & {
		if(self.members.size() > self.max_size) throw std::length_error{"ERROR: enums only support up to " + std::to_string(self.max_size) + " members"};
		if(self.members.empty()) throw std::logic_error{"ERROR: empty enums are not supported"};
		for(const auto & doc : self.lines) out << '\t' << doc << '\n';
		out << "\tenum class " << self.name << " : ::cwc::uint8 {\n";
		auto it = std::begin(self.members);
		for(; it != std::end(self.members); ++it) {
			out << *it;
			if(it + 1 != std::end(self.members)) out << ',';
			out << '\n';
		}
		out << "\t};";
		return out;
	}

	auto operator<<(std::ostream & out, const struct_::member & self) -> std::ostream & {
		out << self.type << ' ' << self.fields[0].name;
		for(const auto & s : self.fields[0].sizes) out << '[' << s << ']';
		for(std::size_t i{1}; i < self.fields.size(); ++i) {
			out << ", " << self.fields[i].name;
			for(const auto & s : self.fields[i].sizes) out << '[' << s << ']';
		}
		out << ';';
		return out;
	}

	auto operator<<(std::ostream & out, const struct_ & self) -> std::ostream & {
		out << "\tCWC_PACK_BEGIN\n";
		for(const auto & doc : self.lines) out << '\t' << doc << '\n';
		out << "\tstruct " << self.name << " {\n";
		const auto indent = self.union_ ? 3 : 2;
		if(self.union_) {
			enum_ e;
			e.name = "cwc_field";
			e.members.push_back({{}, "cwc_no_active_field"});
			for(const auto & member : self.members)
				for(const auto & m : member.fields)
					e.members.push_back({{}, m.name});
			std::stringstream ss;
			ss << e;
			ss.seekp(-1, std::ios::cur);
			ss << " cwc_discriminator;\n";
			std::copy(std::istream_iterator<line>{ss}, std::istream_iterator<line>{}, std::ostream_iterator<std::string>{out << '\t', "\n\t"});
			out << "\tunion {\n";
		}

		for(auto & member : self.members) {
			for(const auto & doc : member.lines) {
				std::fill_n(std::ostream_iterator<char>{out}, indent, '\t');
				out << doc << '\n';
			}
			std::fill_n(std::ostream_iterator<char>{out}, indent, '\t');
			out << member << '\n';
		}

		if(self.union_) out << "\t\t};\n";
		out << "\t};\n"
			"\tCWC_PACK_END\n";

		if(!self.members.empty()) out << '\n';
		for(const auto & member : self.members)
			for(const auto & m : member.fields)
				out << "\tstatic_assert(!::cwc::internal::is_component<std::remove_all_extents<decltype(" << self.name << "::" << m.name << ")>::type>::value, \"Type of '" << self.name << "::" << m.name << "' is incompatible with the requirements of portable structs or unions\");\n";
		if(!self.members.empty()) out << '\n';
		//TODO: generates one linebreak too much!
		return out;
	}

	auto operator<<(std::ostream & out, const typedef_ & self) -> std::ostream & {
		for(const auto & doc : self.lines) out << '\t' << doc << '\n';
		out << "\tusing " << self.name << " = " << self.mutable_ << self.type;
		if(self.array) out << " *";
		out << ";\n"
			"\tstatic_assert(std::is_standard_layout<" << self.type << ">::value, \"Type '" << self.type << "' is incompatible with the requirements for portable type definitions\");";
		return out;
	}

	auto component::factory() const -> interface {
		interface p;
		returns ret;
		p.name = "cwc_factory";

		auto transform = [&](const constructor & c) {
			interface::method m;
			m.lines = c.lines;
			m.name = "operator()";
			m.in = c.params;
			m.out = ret;
			m.out->mutable_ = true;
			m.out->type = "::cwc::component";
			p.members.push_back(m);
		};

		if(members.empty()) transform(constructor());
		else for(const auto & member : members)
			transform(member);

		return p;
	}

	delegate::operator interface() const {
		interface p;
		p.name = name;
		interface::method m;
		m.lines = lines;
		m.name = "operator()";
		m.mutable_ = mutable_;
		m.in = in;
		p.members.push_back(m);
		return p;
	}
}