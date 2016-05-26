
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <string>

namespace cwcc {
	struct library {
		library(const std::string & name);

		auto definition() const -> const std::string & { return definition_; }
	private:
		std::string definition_;
	};
}