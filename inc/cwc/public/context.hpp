
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	//! @brief interface of the global context
	class context : public component {
		virtual
		auto CWC_CALL cwc$context$exception$0(const internal::error * err) const noexcept -> const internal::error * =0;
		virtual
		auto CWC_CALL cwc$context$config$1(intrusive_ptr<config_sections_enumerator> * cwc_ret) const noexcept -> const internal::error * =0;
		virtual
		auto CWC_CALL cwc$context$factory$2(const string_ref * fqn, const optional<const string_ref> * id, intrusive_ptr<component> * cwc_ret) const noexcept -> const internal::error * =0;

		friend
		void internal::validate(const internal::error * err);

		friend
		auto internal::store_exception(std::exception_ptr eptr) noexcept -> const internal::error *;

		auto exception(const internal::error & err) const -> const internal::error * { return cwc$context$exception$0(&err); }
	public:
		//! @brief get read-only access to the current configuration
		//! @returns enumerator for all sections
		auto config() const -> intrusive_ptr<config_sections_enumerator> {
			intrusive_ptr<config_sections_enumerator> cwc_ret;
			internal::call(*this, &context::cwc$context$config$1, &cwc_ret);
			return cwc_ret;
		}

		//! @brief create factory for component type
		//! @tparam Component component type to create factory for
		//! @param[in] id optional id of the plugin
		//! @returns factory for requested type
		template<typename Component>
		auto factory(const optional<const string_ref> & id = nullopt) const -> intrusive_ptr<typename Component::cwc_factory> {
			intrusive_ptr<component> cwc_ret;
			const auto & fqn = Component::cwc_fqn();
			internal::call(*this, &context::cwc$context$factory$2, &fqn, &id, &cwc_ret);
			return cwc_ret;
		}

		//! @brief test if plugin-component is available
		//! @tparam Component component implemented by the plugin
		//! @param[in] id identifier qualifying the tested implementation
		//! @returns true iff implementation is available
		template<typename Component>
		auto plugin_available(const string_ref & id) const -> bool try {
			factory<Component>(id);
			return true;
		} catch(...) {
			return false;
		}
	};

	namespace internal {
		template<>
		struct interface_id<context> final : uuid_constant<0xEA, 0x2E, 0xA7, 0x81, 0x7F, 0xF8, 0x5D, 0xB8, 0xBE, 0xC1, 0xC1, 0x2, 0x4A, 0x74, 0x4E, 0xC8> {};

		template<typename Implementation, typename TypeList>
		class vtable_implementation<context, Implementation, TypeList> : public default_implementation_chaining<Implementation, TypeList> {
			auto CWC_CALL cwc$context$exception$0(const error * err) const noexcept -> const error * final { return static_cast<const Implementation &>(*this).exception(err); }
			auto CWC_CALL cwc$context$config$1(intrusive_ptr<config_sections_enumerator> * cwc_ret) const noexcept -> const error * final { return call_and_return_error([&] { *cwc_ret = static_cast<const Implementation &>(*this).config(); }); }
			auto CWC_CALL cwc$context$factory$2(const string_ref * fqn, const optional<const string_ref> * id, intrusive_ptr<component> * cwc_ret) const noexcept -> const error * final { return call_and_return_error([&] { *cwc_ret = static_cast<const Implementation &>(*this).factory(*fqn, *id); }); }
			//detect missing methods:
			void error();
			void config();
			void factory();
		};
	}

	//! @brief retrieve global context
	auto this_context() -> intrusive_ptr<context>;
}
