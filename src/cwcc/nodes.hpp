
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
	class mutability final {
		bool value{false};
	public:
		mutability() =default;
		mutability(bool value) : value{value} {}

		explicit
		operator bool() const noexcept { return value; }

		friend
		auto operator==(const mutability & lhs, const mutability & rhs) -> bool { return lhs.value == rhs.value; }
		friend 
		auto operator<<(std::ostream & os, const mutability & self) -> std::ostream & { return os << (self.value ? "" : "const "); }
	};

	struct untemplated_type final {
		std::string name;

		friend
		auto operator==(const untemplated_type & lhs, const untemplated_type & rhs) -> bool { return lhs.name == rhs.name; }
		friend 
		auto operator<<(std::ostream & os, const untemplated_type & self) -> std::ostream & { return os << self.name; }
	};

	struct array_ref;
	struct array;
	struct optional;
	struct intrusive_ptr;
	struct variant;

	using templated_type = boost::variant<
		untemplated_type,
		boost::recursive_wrapper<array_ref>,
		boost::recursive_wrapper<array>,
		boost::recursive_wrapper<optional>,
		boost::recursive_wrapper<variant>,
		boost::recursive_wrapper<intrusive_ptr>
	>;

	struct array_ref final {
		mutability mutable_;
		templated_type type;

		friend
		auto operator==(const array_ref & lhs, const array_ref & rhs) -> bool { return (lhs.mutable_ == rhs.mutable_) && (lhs.type == rhs.type); }
		friend
		auto operator<<(std::ostream & os, const array_ref & self) -> std::ostream & { return os << "::cwc::array_ref<" << self.mutable_ << self.type << '>'; }
	};

	struct array final {
		templated_type type;
		std::size_t size;

		friend
		auto operator==(const array & lhs, const array & rhs) -> bool { return (lhs.type == rhs.type) && (lhs.size == rhs.size); }
		friend
		auto operator<<(std::ostream & os, const array & self) -> std::ostream & { return os << "::cwc::array<" << self.type << ", " << self.size << '>'; }
	};

	struct optional final {
		templated_type type;

		friend
		auto operator==(const optional & lhs, const optional & rhs) -> bool { return lhs.type == rhs.type; }
		friend
		auto operator<<(std::ostream & os, const optional & self) -> std::ostream & { return os << "::cwc::optional<" << self.type << '>'; }
	};

	struct intrusive_ptr final {
		mutability mutable_;
		templated_type type;

		friend
		auto operator==(const intrusive_ptr & lhs, const intrusive_ptr & rhs) -> bool { return (lhs.mutable_ == rhs.mutable_) && (lhs.type == rhs.type); }
		friend
		auto operator<<(std::ostream & os, const intrusive_ptr & self) -> std::ostream & { return os << "::cwc::intrusive_ptr<" << self.mutable_ << self.type << '>'; }
	};

	struct variant final {
		std::vector<templated_type> types;

		friend
		auto operator==(const variant & lhs, const variant & rhs) -> bool { return lhs.types == rhs.types; }
		friend
		auto operator<<(std::ostream & os, const variant & self) -> std::ostream & {
			assert(!self.types.empty());
			os << "::cwc::variant<" << self.types[0];
			for(std::size_t i{1}; i < self.types.size(); ++i) os << ", " << self.types[i];
			os << '>';
			return os;
		}
	};

	struct documentation final {
		std::string line;

		friend
		auto operator==(const documentation & lhs, const documentation & rhs) -> bool { return lhs.line == rhs.line; }
		friend
		auto operator<<(std::ostream & os, const documentation & self) -> std::ostream & { return os << "//" << self.line; }
	};

	struct param final {
		mutability mutable_;
		templated_type type;
		std::string name;

		friend
		auto operator==(const param & lhs, const param & rhs) -> bool { return (lhs.mutable_ == rhs.mutable_) && (lhs.type == rhs.type) && (lhs.name == rhs.name); }
	};

	struct enum_ final {
		static const std::size_t max_size{256};

		struct member final {
			std::vector<documentation> lines;
			std::string name;

			friend
			auto operator==(const member & lhs, const member & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.name == rhs.name); }
			friend
			auto operator<<(std::ostream & os, const member & self) -> std::ostream &;
		};

		std::vector<documentation> lines;
		std::string name;
		std::vector<member> members;

		friend
		auto operator==(const enum_ & lhs, const enum_ & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.name == rhs.name) && (lhs.members == rhs.members); }
		friend
		auto operator<<(std::ostream & os, const enum_ & self) -> std::ostream &;
	};

	struct struct_ final {
		struct member final {
			std::vector<documentation> lines;
			templated_type type;
			std::vector<std::string> fields;
	
			friend
			auto operator==(const member & lhs, const member & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.type == rhs.type) && (lhs.fields == rhs.fields); }
			friend
			auto operator<<(std::ostream & os, const member & self) -> std::ostream &;
		};
		
		std::vector<documentation> lines;
		std::string name;
		std::vector<member> members;
	
		friend
		auto operator==(const struct_ & lhs, const struct_ & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.name == rhs.name) && (lhs.members == rhs.members); }
		friend
		auto operator<<(std::ostream & os, const struct_ & self) -> std::ostream &;
	};

	struct typedef_ final {
		std::vector<documentation> lines;
		std::string name;
		templated_type type;

		friend
		auto operator==(const typedef_ & lhs, const typedef_ & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.name == rhs.name) && (lhs.type == rhs.type); }
		friend
		auto operator<<(std::ostream & os, const typedef_ & self) -> std::ostream &;
	};

	struct interface final {
		struct method final {
			std::vector<documentation> lines;
			std::string name;
			std::vector<param> in;
			mutability mutable_;
			boost::optional<templated_type> out;

			auto params() const -> std::vector<param> {
				auto result = in;
				if(out) {
					param p;
					p.name = "cwc_ret";
					p.mutable_ = true;
					p.type = *out;
					result.push_back(p);
				}
				return result;
			}

			friend
			auto operator==(const method & lhs, const method & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.name == rhs.name) && (lhs.in == rhs.in) && (lhs.mutable_ == rhs.mutable_) && (lhs.out == rhs.out); }
		};
		std::vector<documentation> lines;
		std::string name;
		std::vector<method> methods;

		friend
		auto operator==(const interface & lhs, const interface & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.name == rhs.name) && (lhs.methods == rhs.methods); }
	};

	struct component final {
		struct constructor final {
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

	struct enumerator final {
		std::vector<documentation> lines;
		std::string name;
		templated_type type;

		friend
		auto operator==(const enumerator & lhs, const enumerator & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.name == rhs.name) && (lhs.type == rhs.type); }
	};

	struct bundle final {
		std::vector<documentation> lines;
		std::string name;
		std::vector<boost::variant<component, enum_, struct_, typedef_, interface, enumerator>> members;

		friend
		auto operator==(const bundle & lhs, const bundle & rhs) -> bool { return (lhs.lines == rhs.lines) && (lhs.name == rhs.name) && (lhs.members == rhs.members); }
	};
}

BOOST_FUSION_ADAPT_STRUCT(cwcc::untemplated_type, name)
BOOST_FUSION_ADAPT_STRUCT(cwcc::array, type, size)
BOOST_FUSION_ADAPT_STRUCT(cwcc::array_ref, mutable_, type)
BOOST_FUSION_ADAPT_STRUCT(cwcc::optional, type)
BOOST_FUSION_ADAPT_STRUCT(cwcc::intrusive_ptr, mutable_, type)
BOOST_FUSION_ADAPT_STRUCT(cwcc::variant, types)
BOOST_FUSION_ADAPT_STRUCT(cwcc::documentation, line)
BOOST_FUSION_ADAPT_STRUCT(cwcc::struct_::member, lines, type, fields)
BOOST_FUSION_ADAPT_STRUCT(cwcc::struct_, lines, name, members)
BOOST_FUSION_ADAPT_STRUCT(cwcc::component::constructor, lines, params)
BOOST_FUSION_ADAPT_STRUCT(cwcc::enum_::member, lines, name)
BOOST_FUSION_ADAPT_STRUCT(cwcc::enum_, lines, name, members)
BOOST_FUSION_ADAPT_STRUCT(cwcc::typedef_, lines, name, type)
BOOST_FUSION_ADAPT_STRUCT(cwcc::component, lines, name, interfaces, members)
BOOST_FUSION_ADAPT_STRUCT(cwcc::param, mutable_, type, name)
BOOST_FUSION_ADAPT_STRUCT(cwcc::interface::method, lines, name, in, mutable_, out)
BOOST_FUSION_ADAPT_STRUCT(cwcc::enumerator, lines, name, type)
BOOST_FUSION_ADAPT_STRUCT(cwcc::interface, lines, name, methods)
BOOST_FUSION_ADAPT_STRUCT(cwcc::bundle, lines, name, members)