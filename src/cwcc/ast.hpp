
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <string>
#include <vector>
#include <optional>

namespace cwcc {
	class parser;

	class attribute_list final {
		std::optional<std::string> deprecated, nodiscard;
	public:
		attribute_list(parser & p);

		void generate(std::ostream & os) const;
	};

	class param_list final {
		enum class ref_t { none, lvalue, rvalue };

		struct param final {
			bool const_;
			ref_t ref;
			std::string type, name, trailing_comment;

			param(parser & p);

			void generate_param(std::ostream & os) const;
		};

		void generate_vtable(std::ostream & os, bool definition) const;

		std::vector<param> params;
	public:
		param_list(parser & p);

		void generate_param_passing(std::ostream & os) const;
		void generate_param_list(std::ostream & os) const;

		void generate_vtable_declaration(std::ostream & os) const;
		void generate_vtable_definition(std::ostream & os) const;
		void generate_vtable_definition_paramters(std::ostream & os) const;

		auto empty() const noexcept -> bool { return params.empty(); }
	};

	class comment_list final {
		std::vector<std::string> comments;
	public:
		comment_list(parser & p);

		void generate(std::ostream & os) const;
	};

	class constructor final {
		std::optional<comment_list> clist;
		std::optional<attribute_list> alist;
		std::optional<param_list> plist;
		bool delete_;
	public:
		constructor() =default;
		constructor(parser & p, std::optional<comment_list> clist, std::optional<attribute_list> alist);

		void generate_definition(std::ostream & os, std::string_view class_, std::size_t no) const;

		void generate_vtable_declaration(std::ostream & os, std::size_t no) const;
		void generate_vtable_definition(std::ostream & os, std::string_view fqn, bool last) const;
	};

	class method final {
		bool static_;
		std::optional<comment_list> clist;
		std::optional<attribute_list> alist;
		std::string name;
		std::optional<param_list> plist;
		bool const_;
		bool noexcept_;
		std::optional<std::string> result;
		bool delete_;
	public:
		method(parser & p, std::optional<comment_list> clist, std::optional<attribute_list> alist);

		void generate_definition(std::ostream & os, std::size_t no) const;

		void generate_vtable_declaration(std::ostream & os, std::size_t no) const;
		void generate_vtable_definition(std::ostream & os, std::string_view fqn, bool last) const;
	};

	class component final {
		std::optional<comment_list> clist;
		std::string dll;
		std::optional<attribute_list> alist;
		std::string name;
		bool final_;
		std::vector<constructor> constructors;
		std::vector<method> methods;
	public:
		component(parser & p);

		void generate(std::ostream & os, const std::string & namespace_) const;
	};

	class namespace_ final {
		std::optional<comment_list> clist;
		std::string name;
		std::vector<component> components;
	public:
		namespace_(parser & p);

		void generate(std::ostream & os) const;
	};

	class cwcc final {
		std::vector<namespace_> namespaces;
	public:
		cwcc(parser & p);

		void generate(std::ostream & os) const;
	};
}
