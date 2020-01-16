
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <map>
#include <atomic>
#include <cassert>
#include <typeinfo>
#include <functional>
#include <type_traits>

#include <ptl/array.hpp>
#include <ptl/array_ref.hpp>
#include <ptl/bitset.hpp>
#include <ptl/optional.hpp>
#include <ptl/string_ref.hpp>
#include <ptl/tuple.hpp>
#include <ptl/variant.hpp>
#include <ptl/type_list.hpp>

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

#include "public/error_handle.hpp"
#include "public/intrusive_ptr.hpp"

#include "public/default_implementations.hpp"

#include "public/delegate.hpp"
#include "public/component.hpp"
#include "public/context.hpp"

#include "public/bundle.hpp"
