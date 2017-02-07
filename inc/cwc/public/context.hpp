
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
#if defined(CWC_HOST)
	//! @brief modes of context initialization
	enum class init_mode {
		file,//!< @brief initialize from INI-file
		string//!< @brief initialize from INI-string
	};

	//! @brief initialization of the context
	//! @param[in] mode mode of initialization
	//! @param[in] str string to be used in initialization
	//! @note this function must be called before interacting in any way with the context
	//! @attention this function is only available when building a host application
	//! @attention this function may only be called once
	//! @attention invoking this function multiple times or in parallel is undefined behaviour
	void init(init_mode mode, const char * str);
#endif
	//! @brief interface of the global context
	class context : public component {
		virtual void                 CWC_CALL cwc$internal$context$version$0(string_view * cwc_ret) const noexcept =0;
		virtual void                 CWC_CALL cwc$internal$context$error$1(const string_view * msg) const noexcept =0;
		virtual void                 CWC_CALL cwc$internal$context$error$2(string_view * cwc_ret) const noexcept =0;
		virtual internal::error_code CWC_CALL cwc$internal$context$config$3(intrusive_ptr<config_sections_enumerator> * cwc_ret) const noexcept =0;
		virtual internal::error_code CWC_CALL cwc$internal$context$factory$4(const string_view * fqn, intrusive_ptr<component> * cwc_ret) const noexcept =0;
		virtual internal::error_code CWC_CALL cwc$internal$context$factory$5(const string_view * fqn, const string_view * id, intrusive_ptr<component> * cwc_ret) const noexcept =0;

		friend
		void internal::validate(error_code code);

		template<typename Func>
		friend
		auto internal::call_and_return_error(Func func) noexcept -> error_code;

		void error(const string_view & msg) {
			cwc$internal$context$error$1(&msg);
		}

		auto error() const -> string_view {
			string_view cwc_ret;
			cwc$internal$context$error$2(&cwc_ret);
			return cwc_ret;
		}
	public:
		//! @brief get the version information of the context
		//! @returns version information of the context
		auto version() const -> string_view {
			string_view cwc_ret;
			cwc$internal$context$version$0(&cwc_ret);
			return cwc_ret;
		}

		//! @brief create factory for component type
		//! @tparam Component component type to create factory for
		//! @returns factory for requested type
		template<typename Component>
		auto factory() const -> intrusive_ptr<typename Component::cwc_factory> {
			intrusive_ptr<component> cwc_ret;
			const auto & fqn = Component::cwc_fqn();
			internal::call(*this, &context::cwc$internal$context$factory$4, &fqn, &cwc_ret);
			return cwc_ret;
		}

		//! @brief create factory for plugin component type
		//! @tparam Component component type to create factory for
		//! @param[in] id id of the plugin
		//! @returns factory for requested type
		template<typename Component>
		auto factory(const string_view & id) const -> intrusive_ptr<typename Component::cwc_factory> {
			intrusive_ptr<component> cwc_ret;
			const auto & fqn = Component::cwc_fqn();
			internal::call(*this, &context::cwc$internal$context$factory$5, &fqn, &id, &cwc_ret);
			return cwc_ret;
		}

		//! @brief test if plugin-component is available
		//! @tparam Component component implemented by the plugin
		//! @param[in] id identifier qualifying the tested implementation
		//! @returns true iff implementation is available
		template<typename Component>
		auto plugin_available(const string_view & id) const -> bool try {
			factory<Component>(id);
			return true;
		} catch(...) {
			return false;
		}

		//! @brief get read-only access to the current configuration
		//! @returns enumerator for all sections
		auto config() const -> intrusive_ptr<config_sections_enumerator> {
			intrusive_ptr<config_sections_enumerator> cwc_ret;
			internal::call(*this, &context::cwc$internal$context$config$3, &cwc_ret);
			return cwc_ret;
		}

		template<typename CWCImplementation, typename CWCTypeList>
		class cwc_implementation : public internal::default_implementation_chaining<CWCImplementation, CWCTypeList> {
			void                 CWC_CALL cwc$internal$context$version$0(string_view * cwc_ret) const noexcept final { *cwc_ret = static_cast<const CWCImplementation &>(*this).version(); }
			void                 CWC_CALL cwc$internal$context$error$1(const string_view * msg) const noexcept final { static_cast<const CWCImplementation &>(*this).error(*msg); }
			void                 CWC_CALL cwc$internal$context$error$2(string_view * cwc_ret) const noexcept final { *cwc_ret = static_cast<const CWCImplementation &>(*this).error(); }
			internal::error_code CWC_CALL cwc$internal$context$config$3(intrusive_ptr<config_sections_enumerator> * cwc_ret) const noexcept final { return internal::call_and_return_error([&] { *cwc_ret = static_cast<const CWCImplementation &>(*this).config(); }); }
			internal::error_code CWC_CALL cwc$internal$context$factory$4(const string_view * fqn, intrusive_ptr<component> * cwc_ret) const noexcept final { return internal::call_and_return_error([&] { *cwc_ret = static_cast<const CWCImplementation &>(*this).factory(*fqn); }); }
			internal::error_code CWC_CALL cwc$internal$context$factory$5(const string_view * fqn, const string_view * id, intrusive_ptr<component> * cwc_ret) const noexcept final { return internal::call_and_return_error([&] { *cwc_ret = static_cast<const CWCImplementation &>(*this).factory(*fqn, *id); }); }
		};
		static auto cwc_uuid() -> uuid { return {0xEA, 0x2E, 0xA7, 0x81, 0x7F, 0xF8, 0x5D, 0xB8, 0xBE, 0xC1, 0xC1, 0x2, 0x4A, 0x74, 0x4E, 0xC8}; }
	};

	//! @brief retrieve global context
	//! @attention this function may only be called after the context has been initialized
	auto this_context() -> intrusive_ptr<context>;
}