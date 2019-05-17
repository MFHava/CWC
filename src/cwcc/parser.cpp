
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <unordered_set>
//#define BOOST_SPIRIT_DEBUG
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/repository/include/qi_confix.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/repository/include/qi_confix.hpp>
#include <boost/spirit/repository/include/qi_distinct.hpp>
#include "nodes.hpp"
#include "parser.hpp"

namespace {
	struct auto_method {
		std::vector<cwcc::documentation> lines;
		std::string name;
		std::vector<cwcc::param> in;
		cwcc::mutability mutable_;
		cwcc::templated_type out;

		operator cwcc::interface::method() const {
			cwcc::interface::method m;
			m.lines = lines;
			m.name = name;
			m.in = in;
			m.mutable_ = mutable_;
			m.out = out;
			return m;
		}
	};

	struct void_method {
		std::vector<cwcc::documentation> lines;
		std::string name;
		std::vector<cwcc::param> in;
		cwcc::mutability mutable_;

		operator cwcc::interface::method() const {
			cwcc::interface::method m;
			m.lines = lines;
			m.name = name;
			m.in = in;
			m.mutable_ = mutable_;
			return m;
		}
	};
}

BOOST_FUSION_ADAPT_STRUCT(auto_method, lines, name, in, mutable_, out)
BOOST_FUSION_ADAPT_STRUCT(void_method, lines, name, in, mutable_)

namespace {
	using namespace boost::spirit;
	namespace phx = boost::phoenix;

	//keyword[] adapted/taken from http://www.boost.org/doc/libs/1_54_0/libs/spirit/repository/test/qi/distinct.cpp

	namespace traits {
		template<typename Tail>
		struct distinct_spec : result_of::terminal<repository::tag::distinct(Tail)> {};

		template<typename String>
		struct char_spec : result_of::terminal<tag::ascii::char_(String)> {};
	}

	template<typename Tail>
	inline typename traits::distinct_spec<Tail>::type distinct_spec(Tail const & tail) { return repository::distinct(tail); }

	template<typename String>
	inline typename traits::char_spec<String>::type char_spec(String const & str) { return ascii::char_(str); }

	const std::string keyword_spec{"0-9a-zA-Z_"};
	const traits::distinct_spec<traits::char_spec<std::string>::type>::type keyword = distinct_spec(char_spec(keyword_spec)); 

	template<typename Iterator, typename Skipper>
	struct bundle_parser : qi::grammar<Iterator, cwcc::bundle(), Skipper> {
		bundle_parser() : bundle_parser::base_type(start) {
			//NOTE: at least currently CWCC does not even try to validate identifiers (e.g. reserved words/keywords in C++)
			local_identifier   %= ascii::char_("a-zA-Z_") > *ascii::char_("a-zA-Z_0-9");
			bundle_identifier  %= local_identifier > *(repeat(2)[ascii::char_(':')] > local_identifier);
			global_identifier  %= repeat(2)[ascii::char_(':')] > local_identifier > repeat(2)[ascii::char_(':')] > bundle_identifier;

			new_type           %= local_identifier[phx::bind(&bundle_parser::add_new_type, this, _1)];
			existing_type      %= local_identifier[phx::bind(&bundle_parser::make_global_type, this, _1)] | global_identifier[phx::bind(&bundle_parser::validate_global_type, this, _1)];

			templated_type     %= array | array_ref | bitset | optional | tuple | variant | intrusive_ptr | untemplated_type;
			untemplated_type   %= existing_type;
			array              %= keyword["array"]         > '<' >             templated_type > ',' > qi::uint_      > '>';
			array_ref          %= keyword["array_ref"]     > '<' > mutable_ >> templated_type                        > '>';
			bitset             %= keyword["bitset"]        > '<' >                                    qi::uint_      > '>';
			optional           %= keyword["optional"]      > '<' >             templated_type                        > '>';
			tuple              %= keyword["tuple"]         > '<' >             templated_type % ','                  > '>';
			variant            %= keyword["variant"]       > '<' >             templated_type % ','                  > '>';
			intrusive_ptr      %= keyword["intrusive_ptr"] > '<' > mutable_ >> templated_type                        > '>';

			struct_members     %= *documentation >> templated_type >> local_identifier % ',' > qi::lit(';');
			struct_            %= *documentation >> keyword["struct"] > new_type > '{' > +struct_members > '}';

			enum_member        %= *documentation >> local_identifier;
			enum_              %= *documentation >> keyword["enum"] > new_type > '{' > enum_member % ',' > '}';

			component          %= *documentation >> keyword["component"] > new_type > ':' > existing_type % ',' > '{' > *constructor > '}';

			constructor        %= *documentation >> keyword["constructor"] > params > ';';

			void_method_       %= *documentation >> keyword["void"] > local_identifier > params > mutable_;
			auto_method_       %= *documentation >> keyword["auto"] > local_identifier > params > mutable_ > "->" >> templated_type;
			method             %= (void_method_ | auto_method_) > ';';

			enumerator         %= *documentation >> keyword["enumerator"] > new_type > "->" > templated_type;

			mutable_           %= (keyword["mutable"] >> qi::attr(true)) | qi::attr(false);

			params             %= '(' > -(param % ',') > ')';

			param              %= mutable_ >> templated_type >> local_identifier;

			typedef_           %= *documentation >> keyword["using"] > new_type > '=' > templated_type;

			interface          %= *documentation >> keyword["interface"] > new_type > '{' >> *method > '}';

			cpp_comment        %= repository::confix("//", qi::eol | qi::eoi)[*(*ascii::char_('\\') >> +(~ascii::char_('\\') - qi::eol))];

			documentation      %= cpp_comment;

			start              %= *documentation >> keyword["bundle"] > bundle_identifier[phx::bind(&bundle_parser::set_bundle, this, _1)] > '{' > *((component | enum_ | struct_ | interface | typedef_ | enumerator) > ';') > '}';

			qi::on_error(start, phx::bind(error_handler, _1, _3, _2, _4));//register handler for parser errors

			BOOST_SPIRIT_DEBUG_NODE(documentation);
			BOOST_SPIRIT_DEBUG_NODE(untemplated_type);
			BOOST_SPIRIT_DEBUG_NODE(templated_type);
			BOOST_SPIRIT_DEBUG_NODE(array);
			BOOST_SPIRIT_DEBUG_NODE(array_ref);
			BOOST_SPIRIT_DEBUG_NODE(bitset);
			BOOST_SPIRIT_DEBUG_NODE(optional);
			BOOST_SPIRIT_DEBUG_NODE(tuple);
			BOOST_SPIRIT_DEBUG_NODE(variant);
			BOOST_SPIRIT_DEBUG_NODE(intrusive_ptr);
			BOOST_SPIRIT_DEBUG_NODE(constructor);
			BOOST_SPIRIT_DEBUG_NODE(mutable_);
			BOOST_SPIRIT_DEBUG_NODE(interface);
			BOOST_SPIRIT_DEBUG_NODE(params);
			BOOST_SPIRIT_DEBUG_NODE(param);
			BOOST_SPIRIT_DEBUG_NODE(method);
			BOOST_SPIRIT_DEBUG_NODE(auto_method_);
			BOOST_SPIRIT_DEBUG_NODE(void_method_);
			BOOST_SPIRIT_DEBUG_NODE(enumerator);
			BOOST_SPIRIT_DEBUG_NODE(struct_members);
			BOOST_SPIRIT_DEBUG_NODE(struct_);
			BOOST_SPIRIT_DEBUG_NODE(enum_member);
			BOOST_SPIRIT_DEBUG_NODE(enum_);
			BOOST_SPIRIT_DEBUG_NODE(component);
			BOOST_SPIRIT_DEBUG_NODE(typedef_);
			BOOST_SPIRIT_DEBUG_NODE(start);
			BOOST_SPIRIT_DEBUG_NODE(local_identifier);
			BOOST_SPIRIT_DEBUG_NODE(global_identifier);
			BOOST_SPIRIT_DEBUG_NODE(bundle_identifier);
			BOOST_SPIRIT_DEBUG_NODE(new_type);
			BOOST_SPIRIT_DEBUG_NODE(existing_type);
			BOOST_SPIRIT_DEBUG_NODE(cpp_comment);
		}
	private:
		std::string bundle;
		std::unordered_set<std::string> new_types;

		void set_bundle(const std::string & b) { bundle = "::" + b; }

		void add_new_type(const std::string & type) { if(!new_types.insert(type).second) throw std::invalid_argument{"duplicated new type detected"}; }

		void validate_local_type(const std::string & type) { if(!new_types.count(type)) throw std::invalid_argument{"unknown local type '" + type + "'"}; }

		void make_global_type(std::string & type) {
			validate_local_type(type);
			const char sep[] = "::";
			type.insert(std::begin(type), std::begin(sep), std::end(sep) - 1);
			type.insert(std::begin(type), std::begin(bundle), std::end(bundle));
		}

		void validate_global_type(const std::string & type) {
			const auto it = std::find(type.rbegin(), type.rend(), ':').base() - 2;//end of namespace
			assert(it >= std::begin(type));
			if(static_cast<std::size_t>(std::distance(std::begin(type), it)) != bundle.size()) return;//type is not of current bundle
			if(std::equal(std::begin(bundle), std::end(bundle), std::begin(type), std::end(type))) validate_local_type({it + 2, std::end(type)});
		}

		static void error_handler(Iterator begin, Iterator pos, Iterator end, const boost::spirit::info & msg) {
			auto line = 1;
			auto first = begin;
			for(auto it = begin; it != pos; ++it)
				if(*it == '\n') {
					first = it;
					++first;
					++line;
				}
			const auto last = std::find(pos, end, '\n');
			std::stringstream ss;
			ss << "  line: " << line << ": \"";
			const auto tmp = ss.tellp();
			assert(tmp >= 0);
			const auto offset = std::distance(first, pos) + static_cast<std::size_t>(tmp);
			std::transform(first, last, std::ostream_iterator<char>{ss}, [](char c) { return c == '\t' ? ' ' : c; });
			ss << "\"\n";
			std::fill_n(std::ostream_iterator<char>{ss}, offset, ' ');
			ss << "^=== invalid token (expected: " << msg << ')';
			throw std::runtime_error{ss.str()};
		}

		template<typename ReturnType>
		using rule = qi::rule<Iterator, ReturnType(), Skipper>;

		rule<cwcc::untemplated_type> untemplated_type;
		rule<cwcc::templated_type> templated_type;
		rule<cwcc::array> array;
		rule<cwcc::array_ref> array_ref;
		rule<cwcc::bitset> bitset;
		rule<cwcc::optional> optional;
		rule<cwcc::tuple> tuple;
		rule<cwcc::variant> variant;
		rule<cwcc::intrusive_ptr> intrusive_ptr;
		rule<cwcc::component::constructor> constructor;
		rule<cwcc::mutability> mutable_;
		rule<cwcc::interface> interface;
		rule<std::vector<cwcc::param>> params;
		rule<cwcc::param> param;
		rule<cwcc::interface::method> method;
		rule<auto_method> auto_method_;
		rule<void_method> void_method_;
		rule<cwcc::enumerator> enumerator;
		rule<cwcc::struct_::member> struct_members;
		rule<cwcc::struct_> struct_;
		rule<cwcc::enum_::member> enum_member;
		rule<cwcc::enum_> enum_;
		rule<cwcc::component> component;
		rule<cwcc::typedef_> typedef_;
		rule<cwcc::bundle> start;
		qi::rule<Iterator, cwcc::documentation()> documentation;
		qi::rule<Iterator, std::string()> local_identifier, global_identifier, bundle_identifier, new_type, existing_type, cpp_comment;
	};
}

namespace cwcc {
	auto parse(std::istream & is) -> bundle {
		using iterator = std::istreambuf_iterator<char>;
		bundle result;
		const auto success = qi::phrase_parse(make_default_multi_pass(iterator{is}), make_default_multi_pass(iterator{}), bundle_parser<multi_pass<iterator>, qi::space_type>{}, qi::space, result);
		if(!success) throw std::runtime_error{"parsing failed"};
		return result;
	}
}
