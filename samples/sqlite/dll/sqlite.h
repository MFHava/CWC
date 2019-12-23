
//          Copyright Michael Florian Hava 2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <sqlite3.h>
#include "../cwc.sample.sqlite.cwch"

namespace sample {
	using cwc::sample::sqlite::handler;
	using cwc::sample::sqlite::entry;

	struct sqlite3 {
		sqlite3(cwc::string_ref path);
		~sqlite3() noexcept;

		void execute(cwc::string_ref sql, cwc::array_ref<const entry> args, const handler & callback) const;
	private:
		::sqlite3 * db{nullptr};
	};
}
