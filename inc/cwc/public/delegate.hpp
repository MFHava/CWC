
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include <cassert>

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

		template<typename T>
		using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>; //emulating C++20 feature

		template<typename T>
		using param_t = std::add_pointer_t<std::remove_reference_t<T>>;

		std::conditional_t<std::is_same_v<void, Result>, internal::error_callback(void *, param_t<Params>...) noexcept, internal::error_callback(void *, param_t<Params>..., Result *) noexcept> * dispatch;
		void * context;
	public:
		template<typename Functor>
		delegate(Functor && func) noexcept {
			if constexpr(std::is_pointer_v<Functor>) assert(func);

			using Dispatch = remove_cvref_t<std::conditional_t<std::is_pointer_v<Functor>, std::remove_pointer_t<Functor>, Functor>> *;

			if constexpr(std::is_same_v<void, Result>) dispatch = [](void * context, param_t<Params>... args                 ) noexcept { return internal::call_and_store([&] {           (*reinterpret_cast<Dispatch>(context))(*args...); }); };
			else                                       dispatch = [](void * context, param_t<Params>... args, Result * result) noexcept { return internal::call_and_store([&] { *result = (*reinterpret_cast<Dispatch>(context))(*args...); }); };

			if constexpr(std::is_pointer_v<Functor>) context = reinterpret_cast<void *>(func);
			else                                     context = reinterpret_cast<void *>(std::addressof(func));
		}

		delegate(const delegate &) =delete;
		delegate(delegate &&) noexcept =delete;
		auto operator=(const delegate &) -> delegate & =delete;
		auto operator=(delegate &&) noexcept -> delegate & =delete;

		~delegate() noexcept =default;

		auto operator()(Params &&... args) const -> Result {
			if constexpr(std::is_same_v<void, Result>) internal::rethrow_last_error(dispatch(context, &args...));
			else {
				Result result;
				internal::rethrow_last_error(dispatch(context, &args..., &result));
				return result; //cast to void if necessary, otherwise should result in nop
			}
		}
	};

	template<typename Result, typename... Args>
	delegate(Result(*)(Args...)) -> delegate<Result(Args...)>;

	template<typename Result, typename... Args>
	delegate(Result(*)(Args...) noexcept) -> delegate<Result(Args...)>;

	namespace internal {
		template<typename Func>
		struct deduce_mem_func_ptr;

#define CWC_DEDUCE_MEM_FUNC_PTR(CV, REF, EXCEPT)\
	template<typename Result, typename Class, typename... Args>\
	struct deduce_mem_func_ptr<Result(Class:: *)(Args...) CV REF EXCEPT> final {\
		using type = Result(Args...);\
	}

		CWC_DEDUCE_MEM_FUNC_PTR(     ,  ,        );
		CWC_DEDUCE_MEM_FUNC_PTR(     ,& ,        );
		CWC_DEDUCE_MEM_FUNC_PTR(     ,&&,        );
		CWC_DEDUCE_MEM_FUNC_PTR(const,  ,        );
		CWC_DEDUCE_MEM_FUNC_PTR(const,& ,        );
		CWC_DEDUCE_MEM_FUNC_PTR(const,&&,        );
		CWC_DEDUCE_MEM_FUNC_PTR(     ,  ,noexcept);
		CWC_DEDUCE_MEM_FUNC_PTR(     ,& ,noexcept);
		CWC_DEDUCE_MEM_FUNC_PTR(     ,&&,noexcept);
		CWC_DEDUCE_MEM_FUNC_PTR(const,  ,noexcept);
		CWC_DEDUCE_MEM_FUNC_PTR(const,& ,noexcept);
		CWC_DEDUCE_MEM_FUNC_PTR(const,&&,noexcept);

#undef CWC_DEDUCE_MEM_FUNC_PTR

		template<typename Func>
		using deduce_mem_func_ptr_t = typename deduce_mem_func_ptr<Func>::type;
	}

	template<typename Func>
	delegate(Func) -> delegate<internal::deduce_mem_func_ptr_t<decltype(&Func::operator())>>;

	CWC_PACK_END
}
