
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <tuple>
#include <string>
#include <vector>
#include <optional>

namespace cwcc {
	class parser;

	class attribute_list final {
		std::optional<std::string> deprecated, nodiscard;

		auto view() const noexcept { return std::tie(deprecated, nodiscard); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const attribute_list & lhs, const attribute_list & rhs) noexcept -> bool { return lhs.view() == rhs.view(); } //TODO: [C++20] mark defaulted
	public:
		attribute_list(parser & p);
		attribute_list(std::optional<std::string> deprecated, std::optional<std::string> nodiscard) noexcept : deprecated{std::move(deprecated)}, nodiscard{std::move(nodiscard)} {}

		void generate(std::ostream & os) const;
	};

	enum class ref_t { none, lvalue, rvalue };

	struct param final {
		bool const_;
		std::string type;
		ref_t ref;
		std::string name, trailing_comment;

		param(parser & p);
		param(bool const_, std::string type, ref_t ref, std::string name, std::string trailing_comment) noexcept : const_{const_}, type{std::move(type)}, ref{ref}, name{std::move(name)}, trailing_comment{std::move(trailing_comment)} {}

		void generate_param(std::ostream & os) const;
	private:
		auto view() const noexcept { return std::tie(const_, type, ref, name, trailing_comment); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const param & lhs, const param & rhs) noexcept -> bool { return lhs.view() == rhs.view(); } //TODO: [C++20] mark defaulted
	};

	class param_list final {
		void generate_vtable(std::ostream & os, bool definition) const;

		std::vector<param> params;

		auto view() const noexcept { return std::tie(params); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const param_list & lhs, const param_list & rhs) noexcept -> bool { return lhs.view() == rhs.view(); } //TODO: [C++20] mark defaulted
	public:
		param_list(parser & p);
		param_list(std::vector<param> params) noexcept : params{std::move(params)} {}

		void generate_param_passing(std::ostream & os) const;
		void generate_param_list(std::ostream & os) const;

		void generate_vtable_declaration(std::ostream & os) const;
		void generate_vtable_definition(std::ostream & os) const;
		void generate_vtable_definition_paramters(std::ostream & os) const;

		auto empty() const noexcept -> bool { return params.empty(); }
	};

	class comment_list final {
		std::vector<std::string> comments;

		auto view() const noexcept { return std::tie(comments); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const comment_list & lhs, const comment_list & rhs) noexcept -> bool { return lhs.view() == rhs.view(); } //TODO: [C++20] mark defaulted
	public:
		comment_list(parser & p);
		comment_list(std::vector<std::string> comments) noexcept : comments{std::move(comments)} {}

		void generate(std::ostream & os) const;
	};

	class constructor final {
		std::optional<comment_list> clist;
		std::optional<attribute_list> alist;
		std::optional<param_list> plist;
		bool delete_;

		auto view() const noexcept { return std::tie(clist, alist, plist, delete_); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const constructor & lhs, const constructor & rhs) noexcept -> bool { return lhs.view() == rhs.view(); } //TODO: [C++20] mark defaulted
	public:
		constructor() =default;
		constructor(parser & p, std::optional<comment_list> clist, std::optional<attribute_list> alist);
		constructor(std::optional<param_list> plist, bool delete_) noexcept : plist{std::move(plist)}, delete_{delete_} {}

		void generate_definition(std::ostream & os, std::string_view class_, std::size_t no) const;

		void generate_vtable_declaration(std::ostream & os, std::size_t no) const;
		void generate_vtable_definition(std::ostream & os, bool last) const;
	};

	class method final {
		bool static_;
		std::optional<comment_list> clist;
		std::optional<attribute_list> alist;
		std::string name;
		std::optional<param_list> plist;
		bool const_;
		ref_t ref;
		bool noexcept_;
		std::optional<std::string> result;
		bool delete_;

		auto view() const noexcept { return std::tie(static_, clist, alist, name, plist, const_, ref, noexcept_, result, delete_); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const method & lhs, const method & rhs) noexcept -> bool { return lhs.view() == rhs.view(); } //TODO: [C++20] mark defaulted
	public:
		method(parser & p, std::optional<comment_list> clist, std::optional<attribute_list> alist);
		method(bool static_, std::string name, std::optional<param_list> plist, bool const_, ref_t ref, bool noexcept_, std::optional<std::string> result, bool delete_) noexcept : static_{static_}, name{std::move(name)}, plist{std::move(plist)}, const_{const_}, ref{ref}, noexcept_{noexcept_}, result{std::move(result)}, delete_{delete_} {}

		void generate_definition(std::ostream & os, std::size_t no) const;

		void generate_vtable_declaration(std::ostream & os, std::size_t no) const;
		void generate_vtable_definition(std::ostream & os, bool last) const;
	};

	class component final {
		std::optional<comment_list> clist;
		std::string dll;
		std::optional<attribute_list> alist;
		std::string name;
		bool final_;
		std::vector<constructor> constructors;
		std::vector<method> methods;

		auto view() const noexcept { return std::tie(clist, dll, alist, name, final_, constructors, methods); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const component & lhs, const component & rhs) noexcept -> bool { return lhs.view() == rhs.view(); } //TODO: [C++20] mark defaulted
	public:
		component(parser & p);
		component(std::string dll, std::string name, bool final_, std::vector<constructor> constructors, std::vector<method> methods) noexcept : clist{std::move(clist)}, dll{std::move(dll)}, alist{std::move(alist)}, name{std::move(name)}, final_{final_}, constructors{std::move(constructors)}, methods{std::move(methods)} {}

		void generate(std::ostream & os, const std::string & namespace_) const;
	};

	class namespace_ final {
		std::optional<comment_list> clist;
		std::string name;
		std::vector<component> components;

		auto view() const noexcept { return std::tie(clist, name, components); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const namespace_ & lhs, const namespace_ & rhs) noexcept -> bool { return lhs.view() == rhs.view(); } //TODO: [C++20] mark defaulted
	public:
		namespace_(parser & p);
		namespace_(std::string name, std::vector<component> components) noexcept : clist{std::move(clist)}, name{std::move(name)}, components{std::move(components)} {}

		void generate(std::ostream & os) const;
	};

	class cwcc final {
		std::vector<namespace_> namespaces;

		auto view() const noexcept { return std::tie(namespaces); } //TODO: [C++20] remove as operator== will be defaulted
		friend
		auto operator==(const cwcc & lhs, const cwcc & rhs) noexcept -> bool { return lhs.view() == rhs.view(); } //TODO: [C++20] mark defaulted
	public:
		cwcc(parser & p);
		cwcc(std::vector<namespace_> namespaces) noexcept : namespaces{std::move(namespaces)} {}

		void generate(std::ostream & os) const;
	};
}
