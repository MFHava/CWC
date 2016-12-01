
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include <atomic>
#include <cassert>
#include <utility>
#include <stdexcept>

namespace cwc {
	//! @brief base class for all wrappers, every component can be represented by this class
	struct component {
		struct cwc_interface {
			static auto cwc_uuid() -> uuid { return {0x45, 0x91, 0x4, 0xE, 0xEF, 0xBF, 0x5E, 0x84, 0xA1, 0x58, 0x53, 0x3E, 0xD4, 0xAC, 0xFF, 0x93}; }
			using cwc_wrapper = component;
			virtual internal::error_code CWC_CALL cwc$component$new$0() const noexcept =0;
			virtual internal::error_code CWC_CALL cwc$component$delete$1() const noexcept =0;
			virtual internal::error_code CWC_CALL cwc$component$as$2(const uuid id, void ** result) const noexcept =0;
		};

		template<typename Implementation, typename TypeList>
		class cwc_interface_implementation : public internal::default_implementation_chaining<Implementation, TypeList>, private internal::instance_counter {
			mutable std::atomic<int> cwc_reference_counter{1};
			cwc_interface_implementation(const cwc_interface_implementation &) =delete;
			auto operator=(const cwc_interface_implementation &) -> cwc_interface_implementation & =delete;
		public:
			cwc_interface_implementation() =default;

			virtual internal::error_code CWC_CALL cwc$component$new$0() const noexcept override final { return internal::call_and_return_error([&] { ++cwc_reference_counter; }); }
			virtual internal::error_code CWC_CALL cwc$component$delete$1() const noexcept override final { return internal::call_and_return_error([&] { if(!--cwc_reference_counter) delete static_cast<const Implementation *>(this); }); }
			virtual internal::error_code CWC_CALL cwc$component$as$2(const uuid id, void ** result) const noexcept override final { return internal::call_and_return_error([&] { internal::cast_to_interface<Implementation, typename Implementation::cwc_interfaces>::cast(const_cast<Implementation *>(static_cast<const Implementation *>(this)), id, result); }); }
		};

		//! @brief type of this class
		using cwc_self = component;

		//! @brief wrap an existing mutable component in the wrapper
		//! @param[in] ptr mutable component pointer
		component(cwc_interface * ptr) : ptr{ptr} {}

		//! @brief wrap an existing const component in the wrapper
		//! @param[in] ptr mutable component pointer
		component(const cwc_interface * ptr) : ptr{const_cast<cwc_interface*>(ptr)}, immutable{true} {}

		//! @brief copy constructor - increases the reference count of the component referenced by other
		//! @param[in] other component to copy
		component(const component & other) : ptr{other.ptr}, immutable{other.immutable} { if(ptr) internal::validate(ptr->cwc$component$new$0()); }

		//! @brief move constructor - transfers ownership of the referenced component to the new instance
		//! @param[in] other component to adopt ownership from
		component(component && other) noexcept { cwc_swap(other); }

		//! @brief unifying assignment operator
		//! @param[in] other component to copy/move from
		//! @returns reference to this
		auto operator=(component other) noexcept -> component & { cwc_swap(other); return *this; }

		//! @brief test if wrapper contains instance
		//! @returns true iff wrapper manages instance
		explicit operator bool() const noexcept { return ptr != nullptr; }

		//! @brief test if wrapper contains no instance
		//! @returns true iff wrapper manages no instance
		auto operator!() const noexcept -> bool { return ptr == nullptr; }

		//! @brief destructor - decrements the reference count of the managed component
		virtual ~component() noexcept {
			if(!ptr) return;
			auto result = ptr->cwc$component$delete$1();
			assert(result == internal::error_code::no_error);
		}
	protected:
		template<typename Interface>
		class cwc_reference final {
			Interface * ptr{nullptr};

			cwc_reference(const cwc_reference &) =delete;
			cwc_reference(cwc_reference && other) {
				using std::swap;
				swap(ptr, other.ptr);
			}
			auto operator=(const cwc_reference &) -> cwc_reference & =delete;
			auto operator=(cwc_reference &&) -> cwc_reference & =delete;
			friend struct component;
		public:
			cwc_reference(Interface * ptr) : ptr{ptr} {}

			auto operator->() -> Interface * {
				if(!ptr) throw std::logic_error{"wrapper does not contain instance"};
				return ptr;
			}

			auto compare(const cwc_reference & other) const -> std::ptrdiff_t { return ptr - other.ptr; }

			~cwc_reference() noexcept {
				if(!ptr) return;
				const auto result = ptr->cwc$component$delete$1();
				assert(result == internal::error_code::no_error);
			}
		};

		template<typename Interface>
		auto cwc_marshall() -> cwc_reference<typename Interface::cwc_interface> { return cwc_new_reference<Interface>(); }

		template<typename Interface>
		auto cwc_marshall() const -> cwc_reference<const typename Interface::cwc_interface> { return cwc_new_reference<Interface>(); }

		//! @attention this constructor is only present for technical reasons - it allows compilation, but may never be called at runtime!
		component() noexcept { std::terminate(); }
	private:
		void cwc_swap(component & other) noexcept {
			using std::swap;
			swap(ptr, other.ptr);
			swap(immutable, other.immutable);
		}

		auto cwc_compare_references(const component & other) const -> int8 {
			const auto diff = cwc_marshall<component>().compare(other.cwc_marshall<component>());
			if(diff == 0) return 0;
			else if(diff < 0) return -1;
			else return +1;
		}

		template<typename Interface>
		auto cwc_new_reference() -> typename Interface::cwc_interface * {
			if(immutable) throw std::logic_error{"constness violation detected"};
			return cwc_as<Interface>();
		}

		template<typename Interface>
		auto cwc_new_reference() const -> const typename Interface::cwc_interface * { return cwc_as<Interface>(); }

		template<typename Interface>
		auto cwc_as() const -> typename Interface::cwc_interface * {
			if(!ptr) return nullptr;
			typename Interface::cwc_interface * self{nullptr};
			internal::validate(ptr->cwc$component$as$2(Interface::cwc_interface::cwc_uuid(), reinterpret_cast<void**>(&self)));
			return self;
		}

		cwc_interface * ptr{nullptr};
		bool immutable{false};

		friend auto operator<(const component & lhs, const component & rhs) -> bool;
		friend auto operator==(const component & lhs, const component & rhs) -> bool;

		template<typename Type, bool IsComponent>
		friend struct internal::marshalled;

		template<typename Interface>
		friend auto component_cast(const component & self) -> const Interface;

		template<typename Interface>
		friend auto component_cast(component & self) -> Interface;
	};

	//! @brief compare components for identity equality
	//! @param lhs first component
	//! @param rhs second component
	//! @returns true if the components are equal
	inline
	auto operator==(const component & lhs, const component & rhs) -> bool { return lhs.cwc_compare_references(rhs) == 0; }

	//! @brief compare components for identity inequality
	//! @param lhs first component
	//! @param rhs second component
	//! @returns true if the components are not equal
	inline
	auto operator!=(const component & lhs, const component & rhs) -> bool { return !(lhs == rhs); }

	//! @brief compare components for identity ordering
	//! @param lhs first component
	//! @param rhs second component
	//! @returns true if lhs <  rhs
	inline
	auto operator< (const component & lhs, const component & rhs) -> bool { return lhs.cwc_compare_references(rhs) <  0; }

	//! @brief compare components for identity ordering
	//! @param lhs first component
	//! @param rhs second component
	//! @returns true if lhs >  rhs
	inline
	auto operator> (const component & lhs, const component & rhs) -> bool { return rhs < lhs; }

	//! @brief compare components for identity ordering
	//! @param lhs first component
	//! @param rhs second component
	//! @returns true if lhs <= rhs
	inline
	auto operator<=(const component & lhs, const component & rhs) -> bool { return !(lhs > rhs); }

	//! @brief compare components for identity ordering
	//! @param lhs first component
	//! @param rhs second component
	//! @returns true if lhs >= rhs
	inline
	auto operator>=(const component & lhs, const component & rhs) -> bool { return !(lhs < rhs); }

	//! @brief cast between component types, the CWC equivalent of dynamic_cast
	//! @tparam Interface target type
	//! @param[in] self object to cast
	//! @returns immutable representation of the object according to the requested interface
	template<typename Interface>
	inline
	auto component_cast(const component & self) -> const Interface { return self.template cwc_new_reference<Interface>(); }

	//! @brief cast between component types, the CWC equivalent of dynamic_cast
	//! @tparam Interface target type
	//! @param[in] self object to cast
	//! @returns mutable representation of the object according to the requested interface
	template<typename Interface>
	inline
	auto component_cast(component & self) -> Interface { return self.template cwc_new_reference<Interface>(); }
}