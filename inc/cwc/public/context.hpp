
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
		virtual void                 CWC_CALL cwc$context$version$0(string_ref * cwc_ret) const noexcept =0;
		virtual void                 CWC_CALL cwc$context$error$1(const string_ref * msg) const noexcept =0;
		virtual void                 CWC_CALL cwc$context$error$2(string_ref * cwc_ret) const noexcept =0;
		virtual internal::error_code CWC_CALL cwc$context$config$3(intrusive_ptr<config_sections_enumerator> * cwc_ret) const noexcept =0;
		virtual internal::error_code CWC_CALL cwc$context$factory$4(const string_ref * fqn, const optional<const string_ref> * id, intrusive_ptr<component> * cwc_ret) const noexcept =0;

		friend
		void internal::validate(error_code code);

		friend
		auto internal::store_exception(std::exception_ptr eptr) noexcept -> error_code;

		void error(const string_ref & msg) {
			cwc$context$error$1(&msg);
		}

		auto error() const -> string_ref {
			string_ref cwc_ret;
			cwc$context$error$2(&cwc_ret);
			return cwc_ret;
		}
	public:
		//! @brief get the version information of the context
		//! @returns version information of the context
		auto version() const -> string_ref {
			string_ref cwc_ret;
			cwc$context$version$0(&cwc_ret);
			return cwc_ret;
		}

		//! @brief get read-only access to the current configuration
		//! @returns enumerator for all sections
		auto config() const -> intrusive_ptr<config_sections_enumerator> {
			intrusive_ptr<config_sections_enumerator> cwc_ret;
			internal::call(*this, &context::cwc$context$config$3, &cwc_ret);
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
			internal::call(*this, &context::cwc$context$factory$4, &fqn, &id, &cwc_ret);
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

		template<typename CWCImplementation, typename CWCTypeList>
		class cwc_implementation : public internal::default_implementation_chaining<CWCImplementation, CWCTypeList> {
			void                 CWC_CALL cwc$context$version$0(string_ref * cwc_ret) const noexcept final { *cwc_ret = static_cast<const CWCImplementation &>(*this).version(); }
			void                 CWC_CALL cwc$context$error$1(const string_ref * msg) const noexcept final { static_cast<const CWCImplementation &>(*this).error(*msg); }
			void                 CWC_CALL cwc$context$error$2(string_ref * cwc_ret) const noexcept final { *cwc_ret = static_cast<const CWCImplementation &>(*this).error(); }
			internal::error_code CWC_CALL cwc$context$config$3(intrusive_ptr<config_sections_enumerator> * cwc_ret) const noexcept final { return internal::call_and_return_error([&] { *cwc_ret = static_cast<const CWCImplementation &>(*this).config(); }); }
			internal::error_code CWC_CALL cwc$context$factory$4(const string_ref * fqn, const optional<const string_ref> * id, intrusive_ptr<component> * cwc_ret) const noexcept final { return internal::call_and_return_error([&] { *cwc_ret = static_cast<const CWCImplementation &>(*this).factory(*fqn, *id); }); }
			//detect missing methods:
			void version();
			void error();
			void config();
			void factory();
		};
		constexpr
		static auto cwc_uuid() -> uuid { return {static_cast<uint8>(0xEA), static_cast<uint8>(0x2E), static_cast<uint8>(0xA7), static_cast<uint8>(0x81), static_cast<uint8>(0x7F), static_cast<uint8>(0xF8), static_cast<uint8>(0x5D), static_cast<uint8>(0xB8), static_cast<uint8>(0xBE), static_cast<uint8>(0xC1), static_cast<uint8>(0xC1), static_cast<uint8>(0x2), static_cast<uint8>(0x4A), static_cast<uint8>(0x74), static_cast<uint8>(0x4E), static_cast<uint8>(0xC8)}; }
	};

	//! @brief retrieve global context
	auto this_context() -> intrusive_ptr<context>;
}