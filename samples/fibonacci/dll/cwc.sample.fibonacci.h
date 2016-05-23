
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "../cwc.sample.fibonacci.cwch"
#include "fibonacci.h"

namespace cwc {
	namespace sample {
		namespace fibonacci {
			using generator$ = ::sample::fibonacci;//binding your component to the implementation defined name used by the system
		}
	}
}
