
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
			//binding component to the implementation defined name used by the system
			//    (there is no need to inherit the actual implementation itself from component_implementation iff intrusive_from_this is NOT needed)
			struct generator$ : cwc::component_implementation<cwc::sample::fibonacci::generator, generator$>, private ::sample::fibonacci {
				//export member functions to component_implementation
				using ::sample::fibonacci::fibonacci;
				using ::sample::fibonacci::calculate;
			};
		}
	}
}
