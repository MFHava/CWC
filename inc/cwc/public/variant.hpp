
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	//TODO: documentation
	struct bad_variant_access : std::exception {
		auto what() const noexcept -> const char * override { return "bad_variant_access"; }
	};

	namespace internal {
		template<typename TypeList>
		struct biggest_type;

		template<typename Head, typename Tail>
		struct biggest_type<TL::type_list<Head, Tail>> final {
			using type = typename std::conditional<
				(sizeof(Head) > sizeof(typename biggest_type<Tail>::type)),
				Head,
				typename biggest_type<Tail>::type
			>::type;
		};

		template<typename Type>
		struct biggest_type<TL::type_list<Type, TL::empty_type_list>> final {
			using type = Type;
		};

		template<typename ResultType, typename TypeList>
		struct visit_dispatch final {
			template<typename Visitor>
			CWC_CXX_RELAXED_CONSTEXPR
			static auto visit(uint8 index,       void * ptr, Visitor && visitor) -> ResultType {
				return index ? visit_dispatch<ResultType, typename TypeList::tail>::visit(index - 1, ptr, std::forward<Visitor>(visitor))
				             : visitor(*reinterpret_cast<      typename TypeList::head *>(ptr));
			}

			template<typename Visitor>
			CWC_CXX_RELAXED_CONSTEXPR
			static auto visit(uint8 index, const void * ptr, Visitor && visitor) -> ResultType {
				return index ? visit_dispatch<ResultType, typename TypeList::tail>::visit(index - 1, ptr, std::forward<Visitor>(visitor))
				             : visitor(*reinterpret_cast<const typename TypeList::head *>(ptr));
			}
		};

		template<typename ResultType>
		struct visit_dispatch<ResultType, TL::empty_type_list> final {
			template<typename Visitor>
			CWC_CXX_RELAXED_CONSTEXPR
			static auto visit(uint8 index, const void * ptr, Visitor && visitor) -> ResultType { throw std::logic_error{"DESIGN-ERROR: invalid dispatch in visit detected (please report this)"}; }
		};

		template<typename TypeList>
		struct is_unique;

		template<typename Head, typename Tail>
		struct is_unique<TL::type_list<Head, Tail>> final {
			enum { value = (TL::find_if<Tail, Head, std::is_same>::value == -1) && is_unique<Tail>::value };
		};

		template<>
		struct is_unique<TL::empty_type_list> final {
			enum { value = 1 };
		};
	}

	CWC_PACK_BEGIN
	//! @brief a type-safe union, storing one of multiple types
	//! @tparam Types all types that may be stored in the variant
	template<typename... Types>
	struct variant final {
		using all_types = typename internal::TL::make_type_list<Types...>::type;
		using default_type = typename internal::TL::at<all_types, 0>::type;

		static_assert(are_abi_compatible<Types...>::value, "Types do not fulfill ABI requirements");
		static_assert(sizeof...(Types) >   0, "A variant cannot store 0 types");
		static_assert(sizeof...(Types) < 128, "A variant stores at most 127 different types");
		static_assert(internal::is_unique<all_types>::value, "variant does not support duplicated types");

		static const int8 variant_npos{-1};

		CWC_CXX_RELAXED_CONSTEXPR
		variant() : type{0} { new(data) default_type{}; }

		variant(const variant & other) : type{other.type} { if(!valueless_by_exception()) other.visit(copy_ctor{data}); }
		variant(variant && other) noexcept : type{other.type} { if(!valueless_by_exception()) other.visit(move_ctor{data}); }

		template<typename Type, typename = typename std::enable_if<!std::is_same<typename std::decay<Type>::type, variant>::value>::type>
		variant(Type && value) noexcept {
			using DecayedType = typename std::decay<Type>::type;
			static_assert(determine_type<DecayedType>::value != variant_npos, "Type is not stored in variant");
			new(data) DecayedType{std::forward<Type>(value)};
			type = determine_type<DecayedType>::value;
		}

		auto operator=(const variant & other) -> variant & {
			if(other.valueless_by_exception()) reset();
			else if(type == other.type) other.visit(copy_assign{data});
			else {
				reset();
				other.visit(copy_ctor{data});
				type = other.type;
			}
			return *this;
		}
		auto operator=(variant && other) noexcept -> variant & {
			if(other.valueless_by_exception()) reset();
			else if(type == other.type) other.visit(move_assign{data});
			else {
				reset();
				other.visit(move_ctor{data});
				type = other.type;
			}
			return *this;
		}

		template<typename Type>
		auto operator=(Type && value) -> typename std::enable_if<!std::is_same<typename std::decay<Type>::type, variant>::value, variant &>::type {
			using DecayedType = typename std::decay<Type>::type;
			static_assert(determine_type<DecayedType>::value != variant_npos, "Type is not stored in variant");
			if(type == determine_type<DecayedType>::value) {
				*reinterpret_cast<DecayedType *>(data) = std::forward<Type>(value);
			} else {
				reset();
				new(data) DecayedType{std::forward<Type>(value)};
				type = determine_type<DecayedType>::value;
			}
			return *this;
		}

		~variant() noexcept { reset(); }

		CWC_CXX_RELAXED_CONSTEXPR
		auto index() const noexcept -> int8 { return type; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto valueless_by_exception() const noexcept -> bool { return index() == variant_npos; }

		template<typename Visitor>
		CWC_CXX_RELAXED_CONSTEXPR
		auto visit(Visitor && visitor) const -> decltype(std::declval<Visitor>()(std::declval<const default_type &>())) {//TODO: should be free function
			if(valueless_by_exception()) throw bad_variant_access{};
			return internal::visit_dispatch<decltype(visit(std::forward<Visitor>(visitor))), all_types>::visit(type, data, std::forward<Visitor>(visitor));
		}

		template<typename Visitor>
		CWC_CXX_RELAXED_CONSTEXPR
		auto visit(Visitor && visitor)       -> decltype(std::declval<Visitor>()(std::declval<      default_type &>())) {//TODO: should be free function
			if(valueless_by_exception()) throw bad_variant_access{};
			return internal::visit_dispatch<decltype(visit(std::forward<Visitor>(visitor))), all_types>::visit(type, data, std::forward<Visitor>(visitor));
		}
	private:
		void reset() noexcept {
			if(type == variant_npos) return;
			visit(dtor{});
			type = variant_npos;
		}

		struct copy_ctor final {
			copy_ctor(void * data) : data{data} {}

			template<typename Type>
			void operator()(const Type & value) const { new(data) Type{value}; }
		private:
			void * data;
		};
		struct move_ctor final {
			move_ctor(void * data) : data{data} {}

			template<typename Type>
			void operator()(Type & value) const { new(data) Type{std::move(value)}; }
		private:
			void * data;
		};

		struct copy_assign final {
			copy_assign(void * data) : data{data} {}

			template<typename Type>
			void operator()(const Type & value) const { *reinterpret_cast<Type *>(data) = value; }
		private:
			void * data;
		};
		struct move_assign final {
			move_assign(void * data) : data{data} {}

			template<typename Type>
			void operator()(Type & value) const { *reinterpret_cast<Type *>(data) = std::move(value); }
		private:
			void * data;
		};

		struct dtor final {
			template<typename Type>
			void operator()(Type & value) const { value.~Type(); }
		};

		template<typename Type>
		using determine_type = internal::TL::find<all_types, Type>;

		uint8 data[sizeof(typename internal::biggest_type<all_types>::type)];
		int8 type{variant_npos};
	};
	CWC_PACK_END

	namespace internal {
		template<typename... Types>
		struct swap_visitor final {
			swap_visitor(variant<Types...> & other) : other{other} {}

			template<typename Type>
			void operator()(Type & value) { return other.visit(subswap<Type>{value}); }
		private:
			template<typename ValueType>
			struct subswap final {
				subswap(ValueType & lhs) : lhs{lhs} {}

				template<typename Type>
				void operator()(Type &) { throw std::logic_error{"DESIGN-ERROR: invalid dispatch in subswap detected (please report this)"}; }
				void operator()(ValueType & rhs) {
					using std::swap;
					swap(lhs, rhs);
				}
			private:
				ValueType & lhs;
			};
			variant<Types...> & other;
		};
	}

	template<typename... Types>
	void swap(variant<Types...> & lhs, variant<Types...> & rhs) noexcept {
		if(lhs.valueless_by_exception() && rhs.valueless_by_exception()) return;
		if(lhs.index() == rhs.index()) lhs.visit(internal::swap_visitor<Types...>{rhs});
		else std::swap(lhs, rhs);
	}

	namespace internal {
		template<template<typename> class Comparator, typename... Types>
		struct compare_visitor final {
			compare_visitor(const variant<Types...> & other) : other{other} {}

			template<typename Type>
			auto operator()(const Type & value) const -> bool { return other.visit(sub_compare_visitor<Type>{value}); }
		private:
			template<typename ValueType>
			struct sub_compare_visitor final {
				sub_compare_visitor(const ValueType & lhs) : lhs{lhs} {}

				template<typename Type>
				auto operator()(const Type &) const -> bool { throw std::logic_error{"DESIGN-ERROR: invalid dispatch in sub_compare_visitor detected (please report this)"}; }
				auto operator()(const ValueType & rhs) const -> bool { return Comparator<ValueType>{}(lhs, rhs); }
			private:
				const ValueType & lhs;
			};
			const variant<Types...> & other;
		};
	}

	template<typename... Types>
	CWC_CXX_RELAXED_CONSTEXPR
	auto operator==(const variant<Types...> & lhs, const variant<Types...> & rhs) -> bool {
		if(lhs.index() != rhs.index()) return false;
		if(lhs.valueless_by_exception()) return true;
		return lhs.visit(internal::compare_visitor<std::equal_to, Types...>{rhs});
	}

	template<typename... Types>
	CWC_CXX_RELAXED_CONSTEXPR
	auto operator!=(const variant<Types...> & lhs, const variant<Types...> & rhs) -> bool {
		if(lhs.index() != rhs.index()) return true;
		if(lhs.valueless_by_exception()) return false;
		return lhs.visit(internal::compare_visitor<std::not_equal_to, Types...>{rhs});
	}

	template<typename... Types>
	CWC_CXX_RELAXED_CONSTEXPR
	auto operator< (const variant<Types...> & lhs, const variant<Types...> & rhs) -> bool {
		if(rhs.valueless_by_exception()) return false;
		if(lhs.valueless_by_exception()) return true;
		if(lhs.index() < rhs.index()) return true;
		if(lhs.index() > rhs.index()) return false;
		return lhs.visit(internal::compare_visitor<std::less, Types...>{rhs});
	}

	template<typename... Types>
	CWC_CXX_RELAXED_CONSTEXPR
	auto operator> (const variant<Types...> & lhs, const variant<Types...> & rhs) -> bool {
		if(lhs.valueless_by_exception()) return false;
		if(rhs.valueless_by_exception()) return true;
		if(lhs.index() > rhs.index()) return true;
		if(lhs.index() < rhs.index()) return false;
		return lhs.visit(internal::compare_visitor<std::greater, Types...>{rhs});
	}

	template<typename... Types>
	CWC_CXX_RELAXED_CONSTEXPR
	auto operator<=(const variant<Types...> & lhs, const variant<Types...> & rhs) -> bool {
		if(lhs.valueless_by_exception()) return true;
		if(rhs.valueless_by_exception()) return false;
		if(lhs.index() < rhs.index()) return true;
		if(lhs.index() > rhs.index()) return false;
		return lhs.visit(internal::compare_visitor<std::less_equal, Types...>{rhs});
	}

	template<typename... Types>
	CWC_CXX_RELAXED_CONSTEXPR
	auto operator>=(const variant<Types...> & lhs, const variant<Types...> & rhs) -> bool {
		if(rhs.valueless_by_exception()) return true;
		if(lhs.valueless_by_exception()) return false;
		if(lhs.index() > rhs.index()) return true;
		if(lhs.index() < rhs.index()) return false;
		return lhs.visit(internal::compare_visitor<std::greater_equal, Types...>{rhs});
	}

	namespace internal {
		struct print_visitor final {
			print_visitor(std::basic_ostream<utf8> & os) : os{os} {}

			template<typename Type>
			void operator()(const Type & value) const { os << value; }
		private:
			std::basic_ostream<utf8> & os;
		};
	}

	template<typename... Types>
	auto operator<<(std::basic_ostream<utf8> & os, const variant<Types...> & self) -> std::basic_ostream<utf8> & {
		if(self.valueless_by_exception()) return os << "<valueless by exception>";
		self.visit(internal::print_visitor{os});
		return os;
	}

	template<typename Type, typename... Types>
	CWC_CXX_RELAXED_CONSTEXPR
	auto holds_alternative(const variant<Types...> & self) noexcept -> bool {
		using Variant = variant<Types...>;
		const auto tmp{internal::TL::find<typename Variant::all_types, Type>::value};
		return (tmp == Variant::variant_npos) ? false : self.index() == tmp;
	}

	namespace internal {
		template<typename TargetType>
		struct get_if_visitor final {
			auto operator()(TargetType & val) const noexcept -> TargetType * { return &val; }

			template<typename Type>
			auto operator()(Type &) const noexcept -> TargetType * { return nullptr; }
		};
	}

	template<typename Type, typename... Types>
	CWC_CXX_RELAXED_CONSTEXPR
	auto get_if(const variant<Types...> & self) noexcept -> const Type * {
		if(!holds_alternative<Type>(self)) return nullptr;
		return self.visit(internal::get_if_visitor<const Type>{});
	}
	template<typename Type, typename... Types>
	CWC_CXX_RELAXED_CONSTEXPR
	auto get_if(      variant<Types...> & self) noexcept ->       Type * {
		if(!holds_alternative<Type>(self)) return nullptr;
		return self.visit(internal::get_if_visitor<Type>{});
	}

	template<typename Type, typename... Types>
	CWC_CXX_RELAXED_CONSTEXPR
	auto get(const variant<Types...> & self) -> const Type & {
		if(auto ptr = get_if<Type>(self)) return *ptr;
		throw bad_variant_access{};
	}
	template<typename Type, typename... Types>
	CWC_CXX_RELAXED_CONSTEXPR
	auto get(      variant<Types...> & self) ->       Type & {
		if(auto ptr = get_if<Type>(self)) return *ptr;
		throw bad_variant_access{};
	}
}