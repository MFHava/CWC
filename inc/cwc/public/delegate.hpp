
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
	//! @tparam Func function type
	//! @note delegates are by design neither copyable nor movable to ensure lifetime guarantees
	//! @attention delegates are not components!
	template<typename Func>
	class delegate;

	template<typename Result, typename... Params>
	class delegate<Result(Params...)> final {
		static_assert(!(std::is_pointer_v<Params> ||...));

		struct empty {};
		static_assert(sizeof(empty) == 1);
		using result_type = std::conditional_t<std::is_same_v<void, Result>, empty, Result>;

		struct cwc_interface {
			virtual
			auto CWC_CALL invoke(std::add_pointer_t<std::remove_reference_t<Params>>... args, result_type * result) const noexcept -> const internal::error * =0;
			virtual
			void CWC_CALL destroy() noexcept =0;
		};
		template<typename Functor>
		struct cwc_implementation final : cwc_interface {
			Functor cwc_functor;

			cwc_implementation(Functor && func) noexcept : cwc_functor{std::move(func)} {}

			auto CWC_CALL invoke(std::add_pointer_t<std::remove_reference_t<Params>>... args, [[maybe_unused]] result_type * result) const noexcept -> const internal::error * final {
				return internal::call_and_return_error([&] {
					if constexpr(std::is_same_v<void, Result>) cwc_functor(*args...);
					else *result = cwc_functor(*args...);
				});
			}
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

		auto operator()(Params...  args) const -> Result {
			result_type result;
			internal::call(*cwc_func, &cwc_interface::invoke, args..., result);
			return (Result)result; //cast to void if necessary, otherwise should result in nop
		}
	};

	template<typename Result, typename... Args>
	delegate(Result(*)(Args...)) -> delegate<Result(Args...)>;

	namespace internal {
		template<typename Func>
		struct deduce_mem_func_ptr;

#define CWC_DEDUCE_MEM_FUNC_PTR(CV, REF, EXCEPT)\
	template<typename Result, typename Class, typename... Args>\
	struct deduce_mem_func_ptr<Result(Class:: *)(Args...) CV REF EXCEPT> final {\
		using type = Result(Args...);\
	}

		CWC_DEDUCE_MEM_FUNC_PTR(              ,  ,        );
		CWC_DEDUCE_MEM_FUNC_PTR(              ,& ,        );
		CWC_DEDUCE_MEM_FUNC_PTR(              ,&&,        );
		CWC_DEDUCE_MEM_FUNC_PTR(const         ,  ,        );
		CWC_DEDUCE_MEM_FUNC_PTR(const         ,& ,        );
		CWC_DEDUCE_MEM_FUNC_PTR(const         ,&&,        );
		CWC_DEDUCE_MEM_FUNC_PTR(volatile      ,  ,        );
		CWC_DEDUCE_MEM_FUNC_PTR(volatile      ,& ,        );
		CWC_DEDUCE_MEM_FUNC_PTR(volatile      ,&&,        );
		CWC_DEDUCE_MEM_FUNC_PTR(const volatile,  ,        );
		CWC_DEDUCE_MEM_FUNC_PTR(const volatile,& ,        );
		CWC_DEDUCE_MEM_FUNC_PTR(const volatile,&&,        );
		CWC_DEDUCE_MEM_FUNC_PTR(              ,  ,noexcept);
		CWC_DEDUCE_MEM_FUNC_PTR(              ,& ,noexcept);
		CWC_DEDUCE_MEM_FUNC_PTR(              ,&&,noexcept);
		CWC_DEDUCE_MEM_FUNC_PTR(const         ,  ,noexcept);
		CWC_DEDUCE_MEM_FUNC_PTR(const         ,& ,noexcept);
		CWC_DEDUCE_MEM_FUNC_PTR(const         ,&&,noexcept);
		CWC_DEDUCE_MEM_FUNC_PTR(volatile      ,  ,noexcept);
		CWC_DEDUCE_MEM_FUNC_PTR(volatile      ,& ,noexcept);
		CWC_DEDUCE_MEM_FUNC_PTR(volatile      ,&&,noexcept);
		CWC_DEDUCE_MEM_FUNC_PTR(const volatile,  ,noexcept);
		CWC_DEDUCE_MEM_FUNC_PTR(const volatile,& ,noexcept);
		CWC_DEDUCE_MEM_FUNC_PTR(const volatile,&&,noexcept);

#undef CWC_DEDUCE_MEM_FUNC_PTR

		template<typename Func>
		using deduce_mem_func_ptr_t = typename deduce_mem_func_ptr<Func>::type;
	}

	template<typename Func>
	delegate(Func) -> delegate<internal::deduce_mem_func_ptr_t<decltype(&Func::operator())>>;

	CWC_PACK_END
}
