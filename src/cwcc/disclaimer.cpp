
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "disclaimer.hpp"

namespace cwcc {
	void program_disclaimer(std::ostream & os) {
		os << "C++ with Components compiler (CWC)\n"
		      "(C) 2012, Michael Florian Hava\n"
		      "\n";
	}

	void file_disclaimer(std::ostream & os, bool modifiable) {
		os << "//This file was generated for CWC by CWCC\n";
		if(!modifiable) os << "//ATTENTION: Do not modify this file as its content is highly dependent on the design of CWC!\n";
	}
}
