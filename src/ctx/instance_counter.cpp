
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cwc/cwc.hpp>
#include <cwc/internal/instance_counter.hpp>

namespace cwc {
	namespace internal {
		instance_counter::instance_counter() {}//no instance counting for host
		instance_counter::~instance_counter() {}//no instance counting for host
	}
}