
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
	class context {
		virtual
		auto CWC_CALL cwc$context$exception$0(const internal::error * err) const noexcept -> const internal::error * =0;
		virtual
		auto CWC_CALL cwc$context$factory$1(const string_ref * fqn, const optional<const string_ref> * id, intrusive_ptr<component> * cwc_ret) const noexcept -> const internal::error * =0;

		friend
		void internal::validate(const internal::error * err);

		friend
		auto internal::store_exception(std::exception_ptr eptr) noexcept -> const internal::error *;

		auto exception(const internal::error & err) const -> const internal::error * { return cwc$context$exception$0(&err); }
	public:
		//! @brief create factory for component type
		//! @tparam Component component type to create factory for
		//! @param[in] id optional id of the plugin
		//! @returns factory for requested type
		template<typename Component>
		auto factory(const optional<const string_ref> & id = std::nullopt) const -> intrusive_ptr<typename Component::cwc_factory> {
			intrusive_ptr<component> cwc_ret;
			const auto & fqn = Component::cwc_fqn();
			internal::call(*this, &context::cwc$context$factory$1, fqn, id, cwc_ret);
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

	//! @brief retrieve global context
	auto this_context() -> const context &;
}
