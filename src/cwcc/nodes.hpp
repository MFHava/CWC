
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <string>
#include <vector>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace cwcc {
	class mutability {
		bool value{false};
	public:
		mutability() =default;
		mutability(bool value) : value{value} {}

		friend
		auto operator==(const mutability & lhs, const mutability & rhs) -> bool { return lhs.value == rhs.value; }
		friend 
		auto operator<<(std::ostream & out, const mutability & self) -> std::ostream & { return out << (self.value ? "" : "const "); }
	};

	struct documentation {
		std::string line;

		friend
		auto operator==(const documentation & lhs, const documentation & rhs) -> bool { return lhs.line == rhs.line; }
		friend
		auto operator<<(std::ostream & out, const documentation & self) -> std::ostream & { return out << "//" << self.line; }
	};

	struct export_ {
		std::string component;

		friend
		auto operator==(const export_ & lhs, const export_ & rhs) -> bool { return lhs.component == rhs.component; }
	};

	struct param {
		mutability mutable_;
		std::string type, name;

		friend
		auto operator==(const param & lhs, const param & rhs) -> bool { return (lhs.mutable_ == rhs.mutable_) && (lhs.type == rhs.type) && (lhs.name == rhs.name); }
	};

	struct enum_ {
		static const std::size_t max_size{256};

		struct member {
			std::vector<documentation> lines;
			std::string name;

			friend
			auto operator==(const member & lhs, const member & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.name == rhs.name); }
			friend
			auto operator<<(std::ostream & out, const member & self) -> std::ostream &;
		};

		std::vector<documentation> lines;
		std::string name;
		std::vector<member> members;

		friend
		auto operator==(const enum_ & lhs, const enum_ & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.name == rhs.name) && (lhs.members == rhs.members); }
		friend
		auto operator<<(std::ostream & out, const enum_ & self) -> std::ostream &;
	};

	struct struct_ {
		struct member {
			std::vector<documentation> lines;
			std::string type;
			std::vector<int> sizes;//NOTE: 0 == not an array!
			std::vector<std::string> fields;
	
			friend
			auto operator==(const member & lhs, const member & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.type == rhs.type) && (lhs.sizes == rhs.sizes) && (lhs.fields == rhs.fields); }
			friend
			auto operator<<(std::ostream & out, const member & self) -> std::ostream &;
		};
		
		std::vector<documentation> lines;
		std::string name;
		std::vector<member> members;
		bool union_{false};
	
		friend
		auto operator==(const struct_ & lhs, const struct_ & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.name == rhs.name) && (lhs.members == rhs.members) && (lhs.union_ == rhs.union_); }
		friend
		auto operator<<(std::ostream & out, const struct_ & self) -> std::ostream &;
	};

	struct typedef_ {
		enum class attributes { array, optional };
		std::vector<documentation> lines;
		std::string name;
		mutability mutable_;
		std::string type;
		boost::optional<attributes> attribute;

		friend
		auto operator==(const typedef_ & lhs, const typedef_ & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.name == rhs.name) && (lhs.mutable_ == rhs.mutable_) && (lhs.type == rhs.type) && (lhs.attribute == rhs.attribute); }
		friend
		auto operator<<(std::ostream & out, const typedef_ & self) -> std::ostream &;
	};

	struct returns {
		mutability mutable_;
		std::string type;

		friend
		auto operator==(const returns & lhs, const returns & rhs) -> bool { return (lhs.mutable_ == rhs.mutable_) && (lhs.type == rhs.type); }
	};

	struct interface {
		struct method {
			std::vector<documentation> lines;
			std::string name;
			std::vector<param> in;
			mutability mutable_;
			boost::optional<returns> out;

			friend
			auto operator==(const method & lhs, const method & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.name == rhs.name) && (lhs.in == rhs.in) && (lhs.mutable_ == rhs.mutable_) && (lhs.out == rhs.out); }
		};
		std::vector<documentation> lines;
		std::string name;
		std::vector<method> members;

		friend
		auto operator==(const interface & lhs, const interface & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.name == rhs.name) && (lhs.members == rhs.members); }
	};

	struct component {
		struct constructor {
			std::vector<documentation> lines;
			std::vector<param> params;
	
			friend
			auto operator==(const constructor & lhs, const constructor & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.params == rhs.params); }
		};
		std::vector<documentation> lines;
		std::string name;
		std::vector<std::string> interfaces;
		std::vector<constructor> members;

		auto factory() const -> interface;

		friend
		auto operator==(const component & lhs, const component & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.name == rhs.name) && (lhs.interfaces == rhs.interfaces) && (lhs.members == rhs.members); }
	};

	struct delegate {
		std::vector<documentation> lines;
		std::string name;
		std::vector<param> in;
		mutability mutable_;

		operator interface() const;

		friend
		auto operator==(const delegate & lhs, const delegate & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.name == rhs.name) && (lhs.in == rhs.in) && (lhs.mutable_ == rhs.mutable_); }
	};

	struct bundle {
		std::vector<documentation> lines;
		std::string name;
		std::vector<boost::variant<export_, component, enum_, struct_, typedef_, delegate, interface>> members;

		friend
		auto operator==(const bundle & lhs, const bundle & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.name == rhs.name) && (lhs.members == rhs.members); }
	};
}

BOOST_FUSION_ADAPT_STRUCT(cwcc::documentation, line)
BOOST_FUSION_ADAPT_STRUCT(cwcc::struct_::member, lines, type, sizes, fields)
BOOST_FUSION_ADAPT_STRUCT(cwcc::struct_, lines, name, members, union_)
BOOST_FUSION_ADAPT_STRUCT(cwcc::export_, component)
BOOST_FUSION_ADAPT_STRUCT(cwcc::component::constructor, lines, params)
BOOST_FUSION_ADAPT_STRUCT(cwcc::enum_::member, lines, name)
BOOST_FUSION_ADAPT_STRUCT(cwcc::enum_, lines, name, members)
BOOST_FUSION_ADAPT_STRUCT(cwcc::typedef_, lines, name, mutable_, type, attribute)
BOOST_FUSION_ADAPT_STRUCT(cwcc::component, lines, name, interfaces, members)
BOOST_FUSION_ADAPT_STRUCT(cwcc::param, mutable_, type, name)
BOOST_FUSION_ADAPT_STRUCT(cwcc::returns, mutable_, type)
BOOST_FUSION_ADAPT_STRUCT(cwcc::interface::method, lines, name, in, mutable_, out)
BOOST_FUSION_ADAPT_STRUCT(cwcc::delegate, lines, name, in, mutable_)
BOOST_FUSION_ADAPT_STRUCT(cwcc::interface, lines, name, members)
BOOST_FUSION_ADAPT_STRUCT(cwcc::bundle, lines, name, members)