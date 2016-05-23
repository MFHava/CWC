
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include <type_traits>

namespace cwc {
	namespace TL {
		struct empty_type_list;

		template<typename Head, typename Tail>
		struct type_list {
			using head = Head;
			using tail = Tail;
		};

		template<typename... Types>
		struct make_type_list;

		template<>
		struct make_type_list<> {
			using type = empty_type_list;
		};

		template<typename Type, typename... Types>
		struct make_type_list<Type, Types...> {
			using type = type_list<
				Type,
				typename make_type_list<Types...>::type
			>;
		};

		template<typename TypeList>
		struct size;

		template<>
		struct size<empty_type_list> {
			enum { value = 0 };
		};

		template<typename Head, typename Tail>
		struct size<type_list<Head, Tail>> {
			enum { value = 1 + size<Tail>::value };
		};

		template<typename TypeList, typename Type>
		struct append;

		template<>
		struct append<empty_type_list, empty_type_list> {
			using type = empty_type_list;
		};

		template<typename Type>
		struct append<empty_type_list, Type> {
			using type = type_list<
				Type,
				empty_type_list
			>;
		};

		template<typename Head, typename Tail>
		struct append<empty_type_list, type_list<Head, Tail>> {
			using type = type_list<
				Head,
				Tail
			>;
		};

		template<typename Head, typename Tail, typename Type>
		struct append<type_list<Head, Tail>, Type> {
			using type = type_list<
				Head,
				typename append<
					Tail,
					Type
				>::type
			>;
		};

		template<typename TypeList, typename Type>
		struct erase;

		template<typename Type>
		struct erase<empty_type_list, Type> {
			using type = empty_type_list;
		};

		template<typename Head, typename Tail>
		struct erase<type_list<Head, Tail>, Head> {
			using type = Tail;
		};

		template<typename Head, typename Tail, typename Type>
		struct erase<type_list<Head, Tail>, Type> {
			using type = type_list<
				Head,
				typename erase<
					Tail,
					Type
				>::type
			>;
		};

		template<typename TypeList, typename Type>
		struct erase_all;

		template<typename Type>
		struct erase_all<empty_type_list, Type> {
			using type = empty_type_list;
		};

		template<typename Head, typename Tail>
		struct erase_all<type_list<Head, Tail>, Head> {
			using type = typename erase_all<
				Tail,
				Head
			>::type;
		};

		template<typename Head, typename Tail, typename Type>
		struct erase_all<type_list<Head, Tail>, Type> {
			using type = type_list<
				Head,
				typename erase_all<
					Tail,
					Type
				>::type
			>;
		};

		template<typename TypeList>
		struct unique;

		template<>
		struct unique<empty_type_list> {
			using type = empty_type_list;
		};

		template<typename Head, typename Tail>
		struct unique<type_list<Head, Tail>> {
			using type = type_list<
				Head,
				typename erase_all<
					typename unique<Tail>::type,
					Head
				>::type
			>;
		};

		template<typename TypeList, typename Type, template<typename, typename> class BinaryPredicate>
		struct find_if;

		template<typename Type, template<typename, typename> class BinaryPredicate>
		struct find_if<empty_type_list, Type, BinaryPredicate> {
			enum { value = -1 };
		};

		template<typename Head, typename Tail, typename Type, template<typename, typename> class BinaryPredicate>
		struct find_if<type_list<Head, Tail>, Type, BinaryPredicate> {
		private:
			enum {
				match = BinaryPredicate<
					Head,
					Type
				>::value
			};
			enum {
				tmp = find_if<
					Tail,
					Type,
					BinaryPredicate
				>::value
			};
		public:
			enum {
				value = match
					? 0
					: tmp == -1
						? -1
						: 1 + tmp
			};
		};

		template<typename TypeList, typename Type, template<typename, typename> class BinaryPredicate>
		class find_if_not {
			template<typename Type1, typename Type2>
			struct NegatedBinaryPredicate {
				enum {
					value = !BinaryPredicate<
						Type1,
						Type2
					>::value
				};
			};
		public:
			enum {
				value = find_if<
					TypeList,
					Type,
					NegatedBinaryPredicate
				>::value
			};
		};

		template<typename TypeList, typename Type>
		struct find {
			enum {
				value = find_if<
					TypeList,
					Type,
					std::is_same
				>::value
			};
		};

		template<typename TypeList, template<typename> class UnaryFunction>
		struct transform;

		template<template<typename> class UnaryFunction>
		struct transform<empty_type_list, UnaryFunction> {
			using type = empty_type_list;
		};

		template<typename Head, typename Tail, template<typename> class UnaryFunction>
		struct transform<type_list<Head, Tail>, UnaryFunction> {
			using type = type_list<
				typename UnaryFunction<Head>::type,
				typename transform<
					Tail,
					UnaryFunction
				>::type
			>;
		};

		template<typename TypeList, int Index>
		struct at;

		template<typename Head, typename Tail>
		struct at<type_list<Head, Tail>, 0> {
			using type = Head;
		};

		template<typename Head, typename Tail, int Index>
		struct at<type_list<Head, Tail>, Index> {
			using type = typename at<
				Tail,
				Index - 1
			>::type;
		};

		template<typename TypeList, typename OldType, typename NewType, bool All>
		struct replace;

		template<typename OldType, typename NewType, bool All>
		struct replace<empty_type_list, OldType, NewType, All> {
			using type = empty_type_list;
		};

		template<typename Tail, typename OldType, typename NewType>
		struct replace<type_list<OldType, Tail>, OldType, NewType, false> {
			using type = type_list<
				NewType,
				Tail
			>;
		};

		template<typename Tail, typename OldType, typename NewType>
		struct replace<type_list<OldType, Tail>, OldType, NewType, true> {//replace_all
			using type = type_list<
				NewType,
				typename replace<
					Tail,
					OldType,
					NewType,
					true
				>::type
			>;
		};

		template<typename Head, typename Tail, typename OldType, typename NewType, bool All>
		struct replace<type_list<Head, Tail>, OldType, NewType, All> {
			using type = type_list<
				Head,
				typename replace<
					Tail,
					OldType,
					NewType,
					All
				>::type
			>;
		};
	}
}