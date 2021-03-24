
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <atomic>
#include <cassert>
#include <typeinfo>
#include <functional>
#include <type_traits>

#include <ptl/array.hpp>
#include <ptl/array_ref.hpp>
#include <ptl/bitset.hpp>
#include <ptl/optional.hpp>
#include <ptl/optional_ref.hpp>
#include <ptl/string.hpp>
#include <ptl/string_ref.hpp>
#include <ptl/tuple.hpp>
#include <ptl/variant.hpp>
#include <ptl/variant_ref.hpp>
#include <ptl/vector.hpp>

#define CWC_HPP_HEADER_INCLUDED

#define CWC_PACK_BEGIN PTL_PACK_BEGIN
#define CWC_PACK_END   PTL_PACK_END

//! @brief public API of C++ with Components
namespace cwc {
	using namespace ptl;

	//! @brief user defined literals
	namespace literals { using namespace ptl::literals; }

	//! @brief internal implementation details
	//! @attention no member of this namespace is part of the public CWC API
	namespace internal { using namespace ptl::internal; }
}

#include "internal/detection.hpp"

#include "public/types.hpp"

#include "internal/traits.hpp"
#include "internal/type_list.hpp"
#include "internal/error_code.hpp"

#include "public/intrusive_ptr.hpp"

#include "internal/make_base_list.hpp"
#include "internal/call.hpp"
#include "internal/default_implementation_chaining.hpp"
#include "internal/cast_to_interface.hpp"

#include "public/delegate.hpp"
#include "public/component.hpp"
#include "public/context.hpp"
#include "public/default_implementations.hpp"
