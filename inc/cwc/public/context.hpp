
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include <memory>

namespace cwc {
	class context final {
		struct pimpl;

		std::unique_ptr<pimpl> self;

		auto factory(error_context & cwc_error, const std::type_info * type, const uuid & id , std::string_view dll) const -> intrusive_ptr<component>;
	public:
		//! @param[in] force_local override OS specific dll-path behavior and load DLLs only relative to host executable
		context(bool force_local = true);
		~context() noexcept;

		context(const context &) =delete;
		context(context &&) noexcept =delete;
		auto operator=(const context &) -> context & =delete;
		auto operator=(context &&) noexcept -> context & =delete;

		template<typename Configuration>
		auto factory() const {
			default_error_context cwc_error;
			return factory<Configuration>(cwc_error);
		}
		template<typename Configuration>
		auto factory(error_context & cwc_error) const {
			using Component = typename Configuration::component;
			using Factory = typename Component::cwc_factory;
			auto tmp{factory(cwc_error, &typeid(Configuration), internal::interface_id_v<Factory>, Configuration::dll)};
			return intrusive_ptr<Factory>(std::move(tmp));
		}

		template<typename Configuration, typename... Args, typename = std::enable_if_t<(std::is_base_of_v<error_context, Args> || ...)>>
		auto create(Args &&... args) const -> intrusive_ptr<typename Configuration::interface> {
			default_error_context cwc_error;
			return create(cwc_error, std::forward<Args>(args)...);
		}
		template<typename Configuration, typename... Args>
		auto create(Args &&... args) const -> intrusive_ptr<typename Configuration::interface> { return factory<Configuration>()->create(std::forward<Args>(args)...); }
	};
}
