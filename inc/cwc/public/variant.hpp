
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	namespace internal {
		template<typename TypeList>
		struct biggest_type;

		template<typename Head, typename Tail>
		struct biggest_type<TL::type_list<Head, Tail>> {
			using type = typename std::conditional<
				(sizeof(Head) > sizeof(typename biggest_type<Tail>::type)),
				Head,
				typename biggest_type<Tail>::type
			>::type;
		};

		template<typename Type>
		struct biggest_type<TL::type_list<Type, TL::empty_type_list>> {
			using type = Type;
		};

		template<typename ResultType, typename TypeList>
		struct visit_dispatch {
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
		struct visit_dispatch<ResultType, TL::empty_type_list> {
			template<typename Visitor>
			CWC_CXX_RELAXED_CONSTEXPR
			static auto visit(uint8 index, const void * ptr, Visitor && visitor) -> ResultType {
				throw std::runtime_error{""};//TODO: bad_variant_access
			}
		};
	}

	CWC_PACK_BEGIN
	//TODO: documentation
	template<typename... Types>
	struct variant final {
		//TODO: static_assert(std::is_standard_layout<Types...>::value, "variant only supports standard layout types");
		static_assert(sizeof...(Types), "A variant cannot store 0 types");
		static_assert(sizeof...(Types) < 128, "A variant stops at most 128 different types");

		using all_types = typename internal::TL::make_type_list<Types...>::type;
		using default_type = typename internal::TL::at<all_types, 0>::type;
		//TODO: static assert that all types are distinct
		//TODO: all operations must be validated!

		CWC_CXX_RELAXED_CONSTEXPR
		variant() : type{0} { new(data) default_type{}; }

		variant(const variant & other) : type{other.type} { if(!valueless_by_exception()) other.visit(copy_ctor{data}); }
		variant(variant && other) noexcept : type{other.type} { if(!valueless_by_exception()) other.visit(move_ctor{data}); }

		template<typename Type, typename = typename std::enable_if<!std::is_same<typename std::decay<Type>::type, variant>::value>::type>
		variant(Type && value) noexcept {
			static_assert(determine_type<Type>::value != invalid_type, "Type is not stored in variant");
			new(data) Type{std::forward<Type>(value)};
			type = determine_type<Type>::value;
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
			static_assert(determine_type<Type>::value != invalid_type, "Type is not stored in variant");
			if(type == determine_type<Type>::value) {
				*reinterpret_cast<Type *>(data) = std::forward<Type>(value);
			} else {
				reset();
				new(data) Type{std::forward<Type>(value)};
				type = determine_type<Type>::value;
			}
			return *this;
		}

		~variant() noexcept { reset(); }

		CWC_CXX_RELAXED_CONSTEXPR
		auto valueless_by_exception() const noexcept -> bool { return type == invalid_type; }

		template<typename Type>
		CWC_CXX_RELAXED_CONSTEXPR
		auto holds() const noexcept -> bool {
			const auto tmp{determine_type<Type>::value};
			return (tmp == invalid_type) ? false : type == tmp;
		}

		template<typename Visitor>
		CWC_CXX_RELAXED_CONSTEXPR
		auto visit(Visitor && visitor) const -> decltype(std::declval<Visitor>()(std::declval<const default_type &>())) {
			if(valueless_by_exception()) throw std::runtime_error{""};//TODO: bad_variant_access
			return internal::visit_dispatch<decltype(visit(std::forward<Visitor>(visitor))), all_types>::visit(type, data, std::forward<Visitor>(visitor));
		}

		template<typename Visitor>
		CWC_CXX_RELAXED_CONSTEXPR
		auto visit(Visitor && visitor)       -> decltype(std::declval<Visitor>()(std::declval<      default_type &>())) {
			if(valueless_by_exception()) throw std::runtime_error{""};//TODO: bad_variant_access
			return internal::visit_dispatch<decltype(visit(std::forward<Visitor>(visitor))), all_types>::visit(type, data, std::forward<Visitor>(visitor));
		}

		void swap(variant & other) noexcept {
			if(valueless_by_exception() && other.valueless_by_exception()) return;
			if(type == other.type) visit(swapper{other});
			else std::swap(*this, other);
		}

		friend
		CWC_CXX_RELAXED_CONSTEXPR
		auto operator==(const variant & lhs, const variant & rhs) -> bool {
			if(lhs.type != rhs.type) return false;
			if(lhs.valueless_by_exception()) return true;
			return lhs.visit(compare<std::equal_to>{rhs});
		}

		friend
		CWC_CXX_RELAXED_CONSTEXPR
		auto operator!=(const variant & lhs, const variant & rhs) -> bool {
			if(lhs.type != rhs.type) return true;
			if(lhs.valueless_by_exception()) return false;
			return lhs.visit(compare<std::not_equal_to>{rhs});
		}

		friend
		CWC_CXX_RELAXED_CONSTEXPR
		auto operator< (const variant & lhs, const variant & rhs) -> bool {
			if(rhs.valueless_by_exception()) return false;
			if(lhs.valueless_by_exception()) return true;
			if(lhs.type < rhs.type) return true;
			if(lhs.type > rhs.type) return false;
			return lhs.visit(compare<std::less>{rhs});
		}

		friend
		CWC_CXX_RELAXED_CONSTEXPR
		auto operator> (const variant & lhs, const variant & rhs) -> bool {
			if(lhs.valueless_by_exception()) return false;
			if(rhs.valueless_by_exception()) return true;
			if(lhs.type > rhs.type) return true;
			if(lhs.type < rhs.type) return false;
			return lhs.visit(compare<std::greater>{rhs});
		}

		friend
		CWC_CXX_RELAXED_CONSTEXPR
		auto operator<=(const variant & lhs, const variant & rhs) -> bool {
			if(lhs.valueless_by_exception()) return true;
			if(rhs.valueless_by_exception()) return false;
			if(lhs.type < rhs.type) return true;
			if(lhs.type > rhs.type) return false;
			return lhs.visit(compare<std::less_equal>{rhs});
		}

		friend
		CWC_CXX_RELAXED_CONSTEXPR
		auto operator>=(const variant & lhs, const variant & rhs) -> bool {
			if(rhs.valueless_by_exception()) return true;
			if(lhs.valueless_by_exception()) return false;
			if(lhs.type > rhs.type) return true;
			if(lhs.type < rhs.type) return false;
			return lhs.visit(compare<std::greater_equal>{rhs});
		}
	private:
		void reset() noexcept {
			if(type == invalid_type) return;
			visit(dtor{});
			type = invalid_type;
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

		template<template<typename> class Comparator>
		struct compare final {
			compare(const variant & other) : other{other} {}

			template<typename Type>
			auto operator()(const Type & value) const -> bool { return other.visit(subcompare<Type>{value}); }
		private:
			template<typename ValueType>
			struct subcompare final {
				subcompare(const ValueType & lhs) : lhs{lhs} {}

				template<typename Type>
				auto operator()(const Type & rhs) const -> bool { return Comparator<ValueType>{}(lhs, rhs); }
			private:
				const ValueType & lhs;
			};
			const variant & other;
		};

		struct swapper final {
			swapper(variant & other) : other{other} {}

			template<typename Type>
			void operator()(Type & value) { return other.visit(subswapper<Type>{value}); }
		private:
			template<typename ValueType>
			struct subswapper final {
				subswapper(ValueType & lhs) : lhs{lhs} {}

				template<typename Type>
				void operator()(Type &) { throw std::logic_error{""};/*TODO*/ }
				void operator()(ValueType & rhs) {
					using std::swap;
					swap(lhs, rhs);
				}
			private:
				ValueType & lhs;
			};
			variant & other;
		};

		template<typename Type>
		using determine_type = internal::TL::find<all_types, Type>;

		static const int8 invalid_type{-1};
		uint8 data[sizeof(typename internal::biggest_type<all_types>::type)];
		int8 type{invalid_type};
	};
	CWC_PACK_END

	template<typename... Types>
	void swap(variant<Types...> & lhs, variant<Types...> & rhs) noexcept { lhs.swap(rhs); }
}