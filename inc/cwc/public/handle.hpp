
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	template<typename>
	struct handle;

	namespace internal {
		template<typename Implementation, typename... Args>
		auto make_handle(Args &&...) -> handle<component>;
	}

	CWC_PACK_BEGIN
	//! @brief a smart pointer managing objects with embedded reference counts
	//! @tparam Type type of the managed object
	template<typename Type>
	struct handle final {
		static_assert(std::is_base_of_v<component, Type>);

		handle() noexcept =default;

		handle(const handle & other) noexcept : ptr{other.ptr} { if(ptr) ptr->cwc$component$new$0(); }
		handle(handle && other) noexcept { swap(other); }

		auto operator=(handle other) -> handle & {
			swap(other);
			return *this;
		}

		~handle() noexcept { if(ptr) ptr->cwc$component$delete$1(); }

		template<typename OtherType, typename = std::enable_if_t<!std::is_const_v<Type> || (std::is_const_v<Type> && std::is_const_v<OtherType>)>>
		operator handle<OtherType>() const & {
			if(!ptr) return {};
			OtherType * result;
			ptr->cwc$component$dynamic_cast$2(&internal::interface_id_v<OtherType>, (void **)(&result));
			if(!result) throw std::bad_cast{};
			return result;
		}

		template<typename OtherType, typename = std::enable_if_t<!std::is_const_v<Type> || (std::is_const_v<Type> && std::is_const_v<OtherType>)>>
		operator handle<OtherType>() && {
			if(!ptr) return {};
			OtherType * result;
			ptr->cwc$component$dynamic_cast_fast$3(&internal::interface_id_v<OtherType>, (void **)(&result));
			if(!result) throw std::bad_cast{};
			ptr = nullptr;
			return result;
		}

		auto operator*() const noexcept -> Type & { return *ptr; }
		auto operator->() const noexcept -> Type * { return ptr; }

		explicit
		operator bool() const noexcept { return ptr != nullptr; }
		auto operator!() const noexcept -> bool { return ptr == nullptr; }

		void reset() noexcept { handle{}.swap(*this); }

		void swap(handle & other) noexcept { std::swap(ptr, other.ptr); }
		friend
		void swap(handle & lhs, handle & rhs) noexcept { lhs.swap(rhs); }

		template<typename OtherType>
		friend
		auto operator==(const handle & lhs, const handle<OtherType> & rhs) noexcept { return identity(lhs) == identity(rhs); }

		template<typename OtherType>
		friend
		auto operator!=(const handle & lhs, const handle<OtherType> & rhs) noexcept { return !(lhs == rhs); }

		template<typename OtherType>
		friend
		auto operator< (const handle & lhs, const handle<OtherType> & rhs) noexcept { return identity(lhs) < identity(rhs); }

		template<typename OtherType>
		friend
		auto operator<=(const handle & lhs, const handle<OtherType> & rhs) noexcept { return !(rhs < lhs); }

		template<typename OtherType>
		friend
		auto operator>(const handle & lhs, const handle<OtherType> & rhs) noexcept { return rhs < lhs; }

		template<typename OtherType>
		friend
		auto operator>=(const handle & lhs, const handle<OtherType> & rhs) noexcept { return !(lhs < rhs); }

		template<typename OtherType>
		friend
		auto operator<<(std::ostream & os, const handle & self) -> std::ostream & { return os << identity(self);  }
	private:
		template<typename T>
		static
		auto identity(const handle<T> & self) noexcept {
			void * result;
			if constexpr(std::is_same_v<Type, component>) result = self.ptr;
			else if(self) self.ptr->cwc$component$dynamic_cast_fast$3(&internal::interface_id_v<component>, &result);
			else result = nullptr;
			return reinterpret_cast<std::size_t>(result);
		}

		template<typename>
		friend
		struct handle;

		friend
		std::hash<handle<Type>>;

		template<typename, typename... Args>
		friend
		auto internal::make_handle(Args &&...) -> handle<component>;

		handle(Type * ptr) noexcept : ptr{ptr} {}

		Type * ptr{nullptr};
	};
	CWC_PACK_END
}

namespace std {
	template<typename Type>
	struct hash<cwc::handle<Type>> {
		auto operator()(const cwc::handle<Type> & self) const noexcept -> std::size_t {
			return self.identity(self);
		}
	};
}
