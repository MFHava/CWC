
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc::internal::TL {
	struct empty_type_list;

	template<typename Head, typename Tail>
	struct type_list {
		using head = Head;
		using tail = Tail;
	};

	template<typename... Types>
	struct make_type_list;

	template<typename... Types>
	using make_type_list_t = typename make_type_list<Types...>::type;

	template<>
	struct make_type_list<> {
		using type = empty_type_list;
	};

	template<typename Type, typename... Types>
	struct make_type_list<Type, Types...> {
		using type = type_list<
			Type,
			make_type_list_t<Types...>
		>;
	};


	template<typename TypeList, typename Type>
	struct append;

	template<typename TypeList, typename Type>
	using append_t = typename append<TypeList, Type>::type;

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
			append_t<
				Tail,
				Type
			>
		>;
	};

	template<typename TypeList, typename Type>
	struct erase_all;

	template<typename TypeList, typename Type>
	using erase_all_t = typename erase_all<TypeList, Type>::type;

	template<typename Type>
	struct erase_all<empty_type_list, Type> {
		using type = empty_type_list;
	};

	template<typename Head, typename Tail>
	struct erase_all<type_list<Head, Tail>, Head> {
		using type = erase_all_t<
			Tail,
			Head
		>;
	};

	template<typename Head, typename Tail, typename Type>
	struct erase_all<type_list<Head, Tail>, Type> {
		using type = type_list<
			Head,
			erase_all_t<
				Tail,
				Type
			>
		>;
	};

	template<typename TypeList>
	struct unique;

	template<typename TypeList>
	using unique_t = typename unique<TypeList>::type;

	template<>
	struct unique<empty_type_list> {
		using type = empty_type_list;
	};

	template<typename Head, typename Tail>
	struct unique<type_list<Head, Tail>> {
		using type = type_list<
			Head,
			erase_all_t<
				unique_t<Tail>,
				Head
			>
		>;
	};

	template<typename TypeList, int Index>
	struct at;

	template<typename TypeList, int Index>
	using at_t = typename at<TypeList, Index>::type;

	template<typename Head, typename Tail>
	struct at<type_list<Head, Tail>, 0> {
		using type = Head;
	};

	template<typename Head, typename Tail, int Index>
	struct at<type_list<Head, Tail>, Index> {
		using type = at_t<
			Tail,
			Index - 1
		>;
	};
}
