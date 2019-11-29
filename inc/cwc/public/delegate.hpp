
//          Copyright Michael Florian Hava 2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	CWC_PACK_BEGIN
	//! @brief generic helper class to pass functors to components
	//! @tparam Params list of parameter types
	//! @note delegates are by design neither copyable nor movable to ensure lifetime guarantees
	//! @attention delegates are not components!
	template<typename... Params>
	class delegate final {
		static_assert(!(std::is_pointer_v<Params> ||...));
		static_assert(!(std::is_reference_v<Params> ||...));

		struct cwc_interface {
			virtual
			auto CWC_CALL invoke(std::add_pointer_t<Params>... args) const noexcept -> const internal::error * =0;
			virtual
			void CWC_CALL destroy() noexcept =0;
		};

		template<typename Functor>
		struct cwc_implementation final : cwc_interface {
			Functor cwc_functor;

			cwc_implementation(Functor && func) noexcept : cwc_functor{std::move(func)} {}

			auto CWC_CALL invoke(std::add_pointer_t<Params>... args) const noexcept -> const internal::error * final { return internal::call_and_return_error([&] { cwc_functor(*args...); }); }

			void CWC_CALL destroy() noexcept final { delete this; }
		};

		cwc_interface * const cwc_func;

		//only argument passing logic may take the address of a delegate
		auto operator&() const noexcept -> const delegate * { return this; }
		auto operator&()       noexcept ->       delegate * { return this; }

		template<typename Type, typename Func, typename... Args>
		friend
		void internal::call(Type &, Func, Args &&...);
	public:
		template<typename Functor>
		delegate(Functor func) : cwc_func{new cwc_implementation<Functor>{std::move(func)}} {}
		delegate(const delegate &) =delete;
		delegate(delegate &&) noexcept =delete;
		auto operator=(const delegate &) -> delegate & =delete;
		auto operator=(delegate &&) noexcept -> delegate & =delete;
		~delegate() noexcept { cwc_func->destroy(); }

		void operator()(std::add_lvalue_reference_t<Params>...  args) const {
			internal::call(*cwc_func, &cwc_interface::invoke, args...);
		}
	};
	CWC_PACK_END
}
