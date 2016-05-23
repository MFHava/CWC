
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	namespace internal {
		template<typename Type, bool IsComponent>
		struct out_param;

		template<typename SimpleType>
		struct out_param<SimpleType, false> {
			SimpleType * instance;
			auto ref() CWC_NOEXCEPT -> SimpleType ** { return &instance; }
			auto deref() CWC_NOEXCEPT -> SimpleType & { return *instance; }
		};

		template<typename SimpleType>
		struct out_param<const SimpleType, false> {
			SimpleType instance;
			auto ref() CWC_NOEXCEPT -> SimpleType * { return &instance; }
			auto deref() CWC_NOEXCEPT -> SimpleType { return instance; }
		};

		template<typename ComponentType>
		struct out_param<ComponentType, true> {
			using interface = typename make_interface<ComponentType>::type;
			interface * instance;
			auto ref() CWC_NOEXCEPT -> interface ** { return &instance; }
			auto deref() CWC_NOEXCEPT -> interface * { return instance; }
		};

		template<typename Type, bool IsComponent = is_component<Type>::value>
		struct marshalled;

		template<typename SimpleType>
		struct marshalled<SimpleType, false> {
			static auto ref(SimpleType & param) CWC_NOEXCEPT -> SimpleType * { return &param; }
		};

		template<typename SimpleType>
		struct marshalled<const SimpleType, false> {
			static auto ref(const SimpleType & param) CWC_NOEXCEPT -> const SimpleType & { return param; }
		};

		template<typename ComponentType>
		struct marshalled<ComponentType, true> {
			static auto ref(ComponentType & param) -> typename make_interface<ComponentType>::type * { return param.template cwc_new_reference<ComponentType>(); }
		};

		template<typename Type, bool IsInterface = is_interface<typename std::remove_pointer<Type>::type>::value>
		struct unmarshalled;

		template<typename SimpleType>
		struct unmarshalled<SimpleType *, false> {
			static auto deref(SimpleType * param) CWC_NOEXCEPT -> SimpleType & {
				assert(param);
				return *param;
			}
		};

		template<typename SimpleType>
		struct unmarshalled<const SimpleType, false> {
			static auto deref(const SimpleType & param) CWC_NOEXCEPT -> const SimpleType & { return param; }
		};

		template<typename InterfaceType>
		struct unmarshalled<InterfaceType *, true> {
			static auto deref(InterfaceType * param) -> typename make_component<InterfaceType>::type { return param; }
		};

		template<typename Type, bool IsComponent = is_component<Type>::value>
		struct abi;

		template<typename SimpleType>
		struct abi<SimpleType, false> {
		private:
			enum { IsConst = std::is_const<SimpleType>::value };
			using NonConstType = typename std::remove_const<SimpleType>::type;
		public:
			//used for vtable
			using in = typename std::conditional<IsConst, SimpleType, SimpleType *>::type;
			using out = typename std::conditional<IsConst, NonConstType *, SimpleType **>::type;
			using ret = typename std::conditional<IsConst, NonConstType, SimpleType &>::type;
			//used for parameters
			using param = out_param<SimpleType, false>;
			using retval = typename std::conditional<IsConst, SimpleType, NonConstType>::type &;
		};

		template<typename ComponentType>
		struct abi<ComponentType, true> {
			//used for vtable
			using in = typename make_interface<ComponentType>::type *;
			using out = typename make_interface<ComponentType>::type **;
			using ret = ComponentType;
			//used for parameters
			using param = out_param<ComponentType, true>;
			using retval = ret;
		};

		template<typename InputParam>
		inline
		auto to_abi(InputParam & param) -> decltype(marshalled<InputParam>::ref(param)) { return marshalled<InputParam>::ref(param); }

		template<typename InputParam>
		inline
		auto to_abi(const InputParam & param) -> decltype(marshalled<const InputParam>::ref(param)) { return marshalled<const InputParam>::ref(param); }

		template<typename OutputParam, bool IsComponent>
		inline
		auto to_abi(out_param<OutputParam, IsComponent> & param) -> decltype(param.ref()) { return param.ref(); }

		template<typename InputParam>
		inline
		auto from_abi(InputParam & param) -> decltype(unmarshalled<InputParam>::deref(param)) { return unmarshalled<InputParam>::deref(param); }

		template<typename OutputParam, bool IsComponent>
		inline
		auto from_abi(out_param<OutputParam, IsComponent> & param) -> decltype(param.deref()) { return param.deref(); }
	}
}