
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	CWC_PACK_BEGIN
	//! @brief helper class to pass functors to components
	//! @tparam Func function type
	//! @note delegates are by design neither copyable nor movable to ensure lifetime guarantees
	//! @attention Delegates are designed to be sink-parameters, so they will dangle if they outlive the passed functor!
	template<typename Func>
	class delegate;

	template<typename Result, typename... Params>
	class delegate<Result(Params...)> final {
		static_assert(!(std::is_pointer_v<Params> ||...));

		struct empty {};
		static_assert(sizeof(empty) == 1);
		using result_type = std::conditional_t<std::is_same_v<void, Result>, empty, Result>;

		struct interface {
			virtual
			void CWC_CALL invoke(error_context * error, std::add_pointer_t<std::remove_reference_t<Params>>... args, result_type * result) const noexcept =0;
			virtual
			void CWC_CALL destroy() noexcept =0;
		};
		template<typename Functor>
		struct implementation final : interface {
			const Functor & functor;

			implementation(const Functor & func) noexcept : functor{func} {}

			void CWC_CALL invoke(error_context * error, std::add_pointer_t<std::remove_reference_t<Params>>... args, result_type * result) const noexcept final {
				error->call_and_store([&] {
					if constexpr(std::is_same_v<void, Result>) {
						(void)result;
						functor(*args...);
					} else *result = functor(*args...);
				});
			}
			void CWC_CALL destroy() noexcept final { delete this; }
		};

		interface * const func;

		//only argument passing logic may take the address of a delegate
		auto operator&() const noexcept -> const delegate * { return this; }
		auto operator&()       noexcept ->       delegate * { return this; }

		friend
		struct error_context;
	public:
		template<typename Functor>
		delegate(const Functor & func) : func{new implementation<Functor>{func}} {}

		delegate(const delegate &) =delete;
		delegate(delegate &&) noexcept =delete;
		auto operator=(const delegate &) -> delegate & =delete;
		auto operator=(delegate &&) noexcept -> delegate & =delete;

		~delegate() noexcept { func->destroy(); }

		auto operator()(Params...  args) const -> Result {
			default_error_context error;
			return (*this)(error, args...);
		}
		auto operator()(error_context & error, Params... args) const -> Result {
			result_type result;
			error.call(*func, &interface::invoke, args..., result);
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
