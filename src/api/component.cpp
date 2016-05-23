
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cwc/cwc.hpp>

namespace cwc {
	component::component(cwc_interface * ptr) : ptr{ptr} {}

	component::component(const cwc_interface * ptr) : ptr{const_cast<cwc_interface*>(ptr)}, immutable{true} {}

	component::component(const component & other) : ptr{other.ptr}, immutable{other.immutable} { if(ptr) internal::validate(ptr->cwc$component$new$0()); }

	component::component(component && other) CWC_NOEXCEPT { cwc_swap(other); }

	auto component::operator=(component other) CWC_NOEXCEPT -> component & { cwc_swap(other); return *this; }

	component::operator bool() const CWC_NOEXCEPT { return ptr != nullptr; }

	auto component::operator!() const CWC_NOEXCEPT -> bool { return ptr == nullptr; }

	component::~component() CWC_NOEXCEPT {
		if(!ptr) return;
		auto result = ptr->cwc$component$delete$1();
		assert(result == internal::error_code::no_error);
	}

	component::component() CWC_NOEXCEPT { std::terminate(); }

	void component::cwc_swap(component & other) CWC_NOEXCEPT {
		using std::swap;
		swap(ptr, other.ptr);
		swap(immutable, other.immutable);
	}

	auto component::cwc_compare_references(const component & other) const -> int8 {
		const auto diff = cwc_marshall<component>().compare(other.cwc_marshall<component>());
		if(diff == 0) return 0;
		else if(diff < 0) return -1;
		else return +1;
	}

	auto operator==(const component & lhs, const component & rhs) -> bool { return lhs.cwc_compare_references(rhs) == 0; }

	auto operator!=(const component & lhs, const component & rhs) -> bool { return !(lhs == rhs); }

	auto operator<(const component & lhs, const component & rhs) -> bool { return lhs.cwc_compare_references(rhs) < 0; }

	auto operator>(const component & lhs, const component & rhs) -> bool { return rhs < lhs; }

	auto operator<=(const component & lhs, const component & rhs) -> bool { return !(lhs > rhs); }

	auto operator>=(const component & lhs, const component & rhs) -> bool { return !(lhs < rhs); }
}