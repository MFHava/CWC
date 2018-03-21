
#          Copyright Michael Florian Hava 2012.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

CMAKE_MINIMUM_REQUIRED(VERSION 3.9)
PROJECT(CWC CXX)

#BEGIN configuration
OPTION(CWC_BUILD_TESTS "Build tests" OFF)
OPTION(CWC_BUILD_SAMPLES "Build samples" OFF)
OPTION(CWC_USE_STATIC_BOOST "Use static or dynamic Boost libraries (initially selects platform default)" ${WIN32})
#END configuration

INCLUDE_DIRECTORIES(inc;external/PTL/inc)
ADD_SUBDIRECTORY(external/PTL)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/static)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

SET(CMAKE_SHARED_LIBRARY_PREFIX_CXX "")
SET(CMAKE_SHARED_LIBRARY_SUFFIX_CXX ".ccb") #compiled CWC bundle

#BEGIN compiler feature detection
SET(CMAKE_CXX_EXTENSIONS OFF)
INCLUDE(WriteCompilerDetectionHeader)
WRITE_COMPILER_DETECTION_HEADER(
	FILE "${CMAKE_CURRENT_SOURCE_DIR}/inc/cwc/internal/compiler_detection.hpp"
	PREFIX CWC
	COMPILERS MSVC GNU Intel
	FEATURES
		cxx_relaxed_constexpr
)
#END compiler feature detection

SET(CWC_VERSION "WIP")

IF(UNIX)
	SET(CWC_DLL_LIBRARY "dl")
ELSE()
	SET(CWC_DLL_LIBRARY "")
ENDIF()

#BEGIN find required Boost libraries
SET(REQUIRED_BOOST_LIBS filesystem system) #TODO: these dependencies are not strictly required if CWCC-main is (slightly) adapted
IF(CWC_BUILD_TESTS)
	SET(REQUIRED_BOOST_LIBS ${REQUIRED_BOOST_LIBS} unit_test_framework)
ENDIF()
IF(REQUIRED_BOOST_LIBS)
	SET(Boost_USE_STATIC_LIBS ${CWC_USE_STATIC_BOOST}) #TODO: add option to let user select kind of boost libs! (static libraries are rather uncommon on Unix)
	SET(Boost_USE_MULTITHREADED ON)
	FIND_PACKAGE(Boost 1.59.0 REQUIRED ${REQUIRED_BOOST_LIBS})
ENDIF()
#END find required Boost libraries

#BEGIN CWC-library
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/cwc/cwc.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/inc/cwc/cwc.hpp @ONLY)
FILE(GLOB_RECURSE CWC_HEADERS "inc/cwc/*")
FILE(GLOB_RECURSE CWC_SOURCES "src/cwc/*")
FILE(GLOB_RECURSE CWC_GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/src/cwc/*)
ADD_LIBRARY(cwc STATIC ${CWC_HEADERS} ${CWC_SOURCES} ${CWC_GENERATED_SOURCES}) #header-only!
SET_TARGET_PROPERTIES(cwc PROPERTIES LINKER_LANGUAGE CXX)
#END CWC-library

#BEGIN CWCC-application
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/cwcc/disclaimer.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/src/cwcc/disclaimer.cpp @ONLY)
FILE(GLOB_RECURSE CWCC_SOURCES "src/cwcc/*")
FILE(GLOB_RECURSE CWCC_GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/src/cwcc/*)
ADD_EXECUTABLE(cwcc ${CWCC_SOURCES} ${CWCC_GENERATED_SOURCES})
TARGET_LINK_LIBRARIES(cwcc ${CWC_DLL_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
SET_SOURCE_FILES_PROPERTIES(src/cwcc/parser.cpp PROPERTIES
	COMPILE_FLAGS "-O3" #NOTE: this only works as VC++ ignores unknown flags; this flag is necessary because otherwise AR from BINUTILS can't handle the this file (gets very big in debug version)
)
#END CWCC-application

FIND_PACKAGE(Doxygen)
IF(Doxygen_FOUND)
	SET(DOXYGEN_PROJECT_NAME "C++ with Components (${CWC_VERSION})")
	SET(DOXYGEN_PROJECT_BRIEF "A portable extension of C++ to support component oriented software enginnering.")
	SET(DOXYGEN_OUTPUT_DIRECTORY "docs")
	SET(DOXYGEN_BUILTIN_STL_SUPPORT YES)
	SET(DOXYGEN_SHOW_USED_FILES NO)
	SET(DOXYGEN_SHOW_FILES NO)
	SET(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
	SET(DOXYGEN_FILE_PATTERNS "*.hpp;*.dox")
	SET(DOXYGEN_EXCLUDE_SYMBOLS "cwc::internal;cwc::internal::*;cwc_*;cwc::TL;cwc::TL::*")
	SET(DOXYGEN_GENERATE_TREEVIEW YES)
	SET(DOXYGEN_MACRO_EXPANSION YES)
	SET(DOXYGEN_PREDEFINED "CWC_HOST")
	SET(DOXYGEN_COLLABORATION_GRAPH NO)
	DOXYGEN_ADD_DOCS(
		Documentation
		"${PROJECT_SOURCE_DIR}/inc;${PROJECT_SOURCE_DIR}/docs"
	)
ENDIF()

IF(CWC_BUILD_TESTS)
	SET(TEST_LIBRARIES ${CWC_DLL_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

	FILE(GLOB_RECURSE CWC_TEST_SOURCES "test/cwc/*")
	ADD_EXECUTABLE(test-cwc ${CWC_TEST_SOURCES})
	IF(NOT CWC_USE_STATIC_BOOST)
		TARGET_COMPILE_DEFINITIONS(test-cwc PRIVATE -DBOOST_TEST_DYN_LINK)
	ENDIF()
	TARGET_LINK_LIBRARIES(test-cwc ${TEST_LIBRARIES})

	FILE(GLOB_RECURSE CWCC_TEST_SOURCES "test/cwcc/*")
	ADD_EXECUTABLE(test-cwcc ${CWCC_TEST_SOURCES})
	IF(NOT CWC_USE_STATIC_BOOST)
		TARGET_COMPILE_DEFINITIONS(test-cwcc PRIVATE -DBOOST_TEST_DYN_LINK)
	ENDIF()
	TARGET_LINK_LIBRARIES(test-cwcc ${TEST_LIBRARIES})
	SET_SOURCE_FILES_PROPERTIES(test/cwcc/shared.cpp PROPERTIES
		COMPILE_FLAGS "-O3" #NOTE: this only works as VC++ ignores unknown flags; this flag is necessary because otherwise AR from BINUTILS can't handle the this file (gets very big in debug version)
	)
ENDIF()

IF(CWC_BUILD_SAMPLES)
	FILE(GLOB_RECURSE CWC_SAMPLE_FIBONACCI_DLL_SOURCES "samples/fibonacci/dll/*.*")
	ADD_LIBRARY(sample-fibonacci-dll SHARED ${CWC_SAMPLE_FIBONACCI_DLL_SOURCES} "samples/fibonacci/cwc.sample.fibonacci.cwch")

	FILE(GLOB_RECURSE CWC_SAMPLE_FIBONACCI_EXE_SOURCES "samples/fibonacci/exe/*.*")
	ADD_EXECUTABLE(sample-fibonacci ${CWC_SAMPLE_FIBONACCI_EXE_SOURCES} "samples/fibonacci/cwc.sample.fibonacci.cwch")
	SET_SOURCE_FILES_PROPERTIES(samples/fibonacci/dll/cwc.sample.fibonacci.cwc PROPERTIES
		LANGUAGE CXX
		COMPILE_FLAGS "-xc++" #NOTE: this only works as VC++ ignores unknown flags; this flag is necessary because otherwise GCC won't compile this file
	)
	TARGET_LINK_LIBRARIES(sample-fibonacci ${CWC_DLL_LIBRARY})
ENDIF()
