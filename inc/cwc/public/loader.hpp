
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
	template<typename Component>
	struct import final {
		template<std::size_t Size>
		explicit
		constexpr //TODO(C++20): should probably be consteval
		import(const char (&literal)[Size]) : dll{literal} {}

		const std::string_view dll;
	};

	class loader final {
		struct pimpl;

		std::unique_ptr<pimpl> self;

		auto factory(error_context & error, const internal::uuid & id , std::string_view dll) const -> handle<component>;
	public:
		//! @param[in] force_local override OS specific dll-path behavior and load DLLs only relative to host executable
		loader(bool force_local = true);
		~loader() noexcept;

		loader(const loader &) =delete;
		loader(loader &&) noexcept =delete;
		auto operator=(const loader &) -> loader & =delete;
		auto operator=(loader &&) noexcept -> loader & =delete;

		template<typename Component>
		auto factory(import<Component> import, error_context error = error_context{error_context::default_buffer{}}) const -> handle<typename Component::cwc_factory> { return factory(error, internal::interface_id_v<typename Component::cwc_factory>, import.dll); }

		template<typename Component, typename... Args>
		auto create(import<Component> import, Args &&... args) const -> handle<component> { return factory(import)->create(std::forward<Args>(args)...); }
	};
}
