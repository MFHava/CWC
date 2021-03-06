
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE CWC Tests
#define WIN32_LEAN_AND_MEAN//suppress #define interface
#include <boost/test/unit_test.hpp>

#define CWC_CONTEXT_INIT_STRING ""
#define CWC_CONTEXT_INIT_IS_NOT_FILE
#include "cwc/host.hpp"