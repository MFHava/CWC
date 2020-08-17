
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
	class loader final {
		struct pimpl;

		std::unique_ptr<pimpl> self;

		auto factory(error_context & error, const std::type_info * type, const internal::uuid & id , std::string_view dll) const -> handle<component>;
	public:
		//! @param[in] force_local override OS specific dll-path behavior and load DLLs only relative to host executable
		loader(bool force_local = true);
		~loader() noexcept;

		loader(const loader &) =delete;
		loader(loader &&) noexcept =delete;
		auto operator=(const loader &) -> loader & =delete;
		auto operator=(loader &&) noexcept -> loader & =delete;

		template<typename Configuration>
		auto factory(error_context error = error_context{error_context::default_buffer{}}) const -> handle<typename Configuration::component::cwc_factory> {
			using Component = typename Configuration::component;
			using Factory = typename Component::cwc_factory;
			return factory(error, &typeid(Configuration), internal::interface_id_v<Factory>, Configuration::dll);
		}

		template<typename Configuration, typename... Args>
		auto create(Args &&... args) const -> handle<typename Configuration::interface> {
			return factory<Configuration>()->create(std::forward<Args>(args)...);
		}
	};
}
