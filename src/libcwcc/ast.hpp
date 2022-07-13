
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <tuple> //TODO: [C++20] remove as view-hack is no longer needed
#include <vector>
#include <variant>
#include <optional>
#include <string_view>

namespace cwcc {
	class parser;

	struct attribute final {
		std::string_view name;
		std::optional<std::string_view> reason;

		void parse(parser & p);
		
		auto view() const noexcept { return std::tie(name, reason); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const attribute & lhs, const attribute & rhs) noexcept -> bool { return lhs.view() == rhs.view(); }
	};

	enum class ref_t { none, lvalue, rvalue };

	struct param final {
		bool const_{false};
		std::string_view type;
		ref_t ref{ref_t::none};
		std::string_view name;

		void parse(parser & p);

		auto view() const noexcept { return std::tie(const_, type, ref, name); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const param & lhs, const param & rhs) noexcept -> bool { return lhs.view() == rhs.view(); } //TODO: [C++20] mark defaulted
	};

	struct comment final {
		std::string_view line;

		void parse(parser & p);

		friend
		auto operator==(const comment & lhs, const comment & rhs) noexcept -> bool { return lhs.line == rhs.line; } //TODO: [C++20] mark defaulted
	};

	struct constructor final {
		std::string_view name;
		std::vector<param> params;
		bool delete_{false};

		void parse(parser & p);

		auto view() const noexcept { return std::tie(name, params, delete_); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const constructor & lhs, const constructor & rhs) noexcept -> bool { return lhs.view() == rhs.view(); } //TODO: [C++20] mark defaulted
	};

	struct method final {
		bool static_{false};
		std::string_view name;
		std::vector<param> params;
		bool const_{false};
		ref_t ref{ref_t::none};
		bool noexcept_{false};
		std::optional<std::string_view> result;
		bool delete_{false};

		void parse(parser & p);

		auto view() const noexcept { return std::tie(static_, name, params, const_, ref, noexcept_, result, delete_); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const method & lhs, const method & rhs) noexcept -> bool { return lhs.view() == rhs.view(); } //TODO: [C++20] mark defaulted
	};

	struct component final {
		std::vector<attribute> attributes;
		std::string_view name;
		bool final{false};
		std::vector<std::variant<comment, constructor, method, attribute>> content;

		void parse(parser & p);

		auto view() const noexcept { return std::tie(attributes, name, final, content); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const component & lhs, const component & rhs) noexcept -> bool { return lhs.view() == rhs.view(); } //TODO: [C++20] mark defaulted
	};

	struct template_ final {
		std::vector<std::string_view> tparams;
		component component_;

		void parse(parser & p);

		auto view() const noexcept { return std::tie(tparams, component_); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const template_ & lhs, const template_ & rhs) noexcept -> bool { return lhs.view() == rhs.view(); } //TODO: [C++20] mark defaulted
	};

	struct extern_ final {
		std::string_view component;
		std::vector<std::string_view> tparams;

		void parse(parser & p);

		auto view() const noexcept { return std::tie(component, tparams); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const extern_ & lhs, const extern_ & rhs) noexcept -> bool { return lhs.view() == rhs.view(); } //TODO: [C++20] mark defaulted
	};

	struct library final {
		std::string_view name;
		std::variant<component, extern_> content;

		void parse(parser & p);

		auto view() const noexcept { return std::tie(name, content); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const library & lhs, const library & rhs) noexcept -> bool { return lhs.view() == rhs.view(); } //TODO: [C++20] mark defaulted
	};

	struct namespace_ final {
		std::string_view name;
		std::vector<std::variant<comment, template_, library>> content;

		void parse(parser & p);

		auto view() const noexcept { return std::tie(name, content); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const namespace_ & lhs, const namespace_ & rhs) noexcept -> bool { return lhs.view() == rhs.view(); } //TODO: [C++20] mark defaulted
	};

	struct include final {
		std::string_view header;

		void parse(parser & p);

		friend
		auto operator==(const include & lhs, const include & rhs) noexcept -> bool { return lhs.header == rhs.header; } //TODO: [C++20] remove as operator== will be defaulted
	};

	struct cwc final {
		std::vector<std::variant<include, comment, namespace_>> content;

		void parse(parser & p);

		friend
		auto operator==(const cwc & lhs, const cwc & rhs) noexcept -> bool { return lhs.content == rhs.content; } //TODO: [C++20] remove as operator== will be defaulted
	};
}
