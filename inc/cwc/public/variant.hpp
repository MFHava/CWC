
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

		template<typename TypeList>
		struct visit_dispatch {
			template<typename Visitor>
			CWC_CXX_RELAXED_CONSTEXPR
			static auto visit(uint8 index,       uint8 * ptr, Visitor && visitor) {
				return index ? visit_dispatch<typename TypeList::tail>::visit(index - 1, ptr, std::forward<Visitor>(visitor))
				             : visitor(*reinterpret_cast<typename TypeList::head *>(ptr));
			}

			template<typename Visitor>
			CWC_CXX_RELAXED_CONSTEXPR
			static auto visit(uint8 index, const uint8 * ptr, Visitor && visitor) {
				return index ? visit_dispatch<typename TypeList::tail>::visit(index - 1, ptr, std::forward<Visitor>(visitor))
				             : visitor(*reinterpret_cast<const typename TypeList::head *>(ptr));
			}
		};

		template<>
		struct visit_dispatch<TL::empty_type_list> {
			template<typename Visitor>
			CWC_CXX_RELAXED_CONSTEXPR
			static auto visit(uint8 index, const uint8 * ptr, Visitor && visitor) {
				throw std::runtime_error{""};//TODO: bad_variant_access
			}
		};
	}
	CWC_PACK_BEGIN

	//TODO: documentation
	template<typename... Types>
	struct variant final {
		static_assert(std::is_standard_layout<Types...>::value, "variant only supports standard layout types");
		static_assert(sizeof...(Types), "A variant cannot store 0 types");
		static_assert(sizeof...(Types) < 128, "A variant stops at most 128 different types");

		using types = typename internal::TL::make_type_list<Types...>::type;
		//TODO: static assert that all types are distinct
		//TODO: all operations must be validated!

		CWC_CXX_RELAXED_CONSTEXPR
		variant() : type{0} { new(data) typename internal::TL::at<types, 0>::type{}; }

		variant(const variant & other) : type{other.type} {
			if(valueless_by_exception()) return;
			other.visit([&](const auto & value) {
				using Type = decltype(value);
				new(data) Type{value};
			});
		}
		variant(variant && other) noexcept : type{other.type} {
			if(valueless_by_exception()) return;
			other.visit([&](auto & value) {
				using Type = decltype(value);
				new(data) Type{std::move(value)};
			});
		}

		template<typename Type>
		variant(Type && value) noexcept : type{determine_type<Type>::value} {
			static_assert(determine_type<Type>::value != invalid_type, "Type is not stored in variant");
			new(data) Type{std::forward<Type>(value)};
		}

		auto operator=(const variant & other) -> variant & {
			if(other.valueless_by_exception()) reset();
			else if(type == other.type)
				other.visit([&](const auto & value) {
				using Type = decltype(value);
				*reinterpret_cast<Type *>(data) = value;
			});
			else {
				reset();
				other.visit([&](const auto & value) {
					using Type = decltype(value);
					new(data) Type{value};
				});
				type = other.type;
			}
			return *this;
		}
		auto operator=(variant && other) noexcept -> variant & {
			if(other.valueless_by_exception()) reset();
			else if(type == other.type)
				other.visit([&](const auto & value) {
				using Type = decltype(value);
				*reinterpret_cast<Type *>(data) = std::move(value);
			});
			else {
				reset();
				other.visit([&](const auto & value) {
					using Type = decltype(value);
					new(data) Type{std::move(value)};
				});
				type = other.type;
			}
			return *this;
		}

		template<typename Type>
		auto operator=(Type && value) -> variant & {
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
		auto visit(Visitor && visitor) const {
			if(valueless_by_exception()) throw std::runtime_error{""};//TODO: bad_variant_access
			return internal::visit_dispatch<types>::visit(type, data, std::forward<Visitor>(visitor));
		}

		template<typename Visitor>
		CWC_CXX_RELAXED_CONSTEXPR
		auto visit(Visitor && visitor)       {
			if(valueless_by_exception()) throw std::runtime_error{""};//TODO: bad_variant_access
			return internal::visit_dispatch<types>::visit(type, data, std::forward<Visitor>(visitor));
		}

		void swap(variant & other) noexcept {
			if(valueless_by_exception() && other.valueless_by_exception()) return;
			if(type == other.type)
				visit([&](auto & l) {
					other.visit([&](auto & r) {
						using std::swap;
						swap(l, r);
					});
				});
			else std::swap(*this, other);
		}

		friend
		CWC_CXX_RELAXED_CONSTEXPR
		auto operator==(const variant & lhs, const variant & rhs) -> bool {
			if(lhs.type != rhs.type) return false;
			if(lhs.valueless_by_exception()) return true;
			return lhs.visit([&](const auto & l) {
				return rhs.visit([&](const auto & r) {
					return l == r;
				});
			});
		}

		friend
		CWC_CXX_RELAXED_CONSTEXPR
		auto operator!=(const variant & lhs, const variant & rhs) -> bool {
			if(lhs.type != rhs.type) return true;
			if(lhs.valueless_by_exception()) return false;
			return lhs.visit([&](const auto & l) {
				return rhs.visit([&](const auto & r) {
					return l != r;
				});
			});
		}

		friend
		CWC_CXX_RELAXED_CONSTEXPR
		auto operator< (const variant & lhs, const variant & rhs) -> bool {
			if(rhs.valueless_by_exception()) return false;
			if(lhs.valueless_by_exception()) return true;
			if(lhs.type < rhs.type) return true;
			if(lhs.type > rhs.type) return false;
			return lhs.visit([&](const auto & l) {
				return rhs.visit([&](const auto & r) {
					return l <  r; 
				});
			});
		}

		friend
		CWC_CXX_RELAXED_CONSTEXPR
		auto operator> (const variant & lhs, const variant & rhs) -> bool {
			if(lhs.valueless_by_exception()) return false;
			if(rhs.valueless_by_exception()) return true;
			if(lhs.type > rhs.type) return true;
			if(lhs.type < rhs.type) return false;
			return lhs.visit([&](const auto & l) {
				return rhs.visit([&](const auto & r) {
					return l >  r;
				});
			});
		}

		friend
		CWC_CXX_RELAXED_CONSTEXPR
		auto operator<=(const variant & lhs, const variant & rhs) -> bool {
			if(lhs.valueless_by_exception()) return true;
			if(rhs.valueless_by_exception()) return false;
			if(lhs.type < rhs.type) return true;
			if(lhs.type > rhs.type) return false;
			return lhs.visit([&](const auto & l) {
				return rhs.visit([&](const auto & r) {
					return l <= r;
				});
			});
		}

		friend
		CWC_CXX_RELAXED_CONSTEXPR
		auto operator>=(const variant & lhs, const variant & rhs) -> bool {
			if(rhs.valueless_by_exception()) return true;
			if(lhs.valueless_by_exception()) return false;
			if(lhs.type > rhs.type) return true;
			if(lhs.type < rhs.type) return false;
			return lhs.visit([&](const auto & l) {
				return rhs.visit([&](const auto & r) {
					return l >= r;
				});
			});
		}
	private:
		void reset() noexcept {
			if(type == invalid_type) return;
			visit([&](auto & value) {
				using Type = decltype(value);
				value.~Type();
				type = invalid_type;
			});
		}

		template<typename Type>
		using determine_type = internal::TL::find<types, Type>;
		static const auto invalid_type{std::numeric_limits<uint8>::max()};
		uint8 data[sizeof(typename internal::biggest_type<types>::type)], type{invalid_type};
	};
	CWC_PACK_END

	template<typename... Types>
	void swap(variant<Types...> & lhs, variant<Types...> & rhs) noexcept { lhs.swap(rhs); }
}