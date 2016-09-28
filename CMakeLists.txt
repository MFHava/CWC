
#          Copyright Michael Florian Hava 2012.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

CMAKE_MINIMUM_REQUIRED(VERSION 3.2.2)
PROJECT(CWC CXX)

#BEGIN configuration
OPTION(CWC_BUILD_TESTS "Build tests" OFF)
OPTION(CWC_BUILD_SAMPLES "Build samples" OFF)
OPTION(CWC_BUILD_DOCUMENTATION "Build documentation" OFF)
OPTION(CWC_USE_BOOST_REGEX "Enable fallback to Boost.Regex if Std.Regex is not available" OFF)
SET(CWC_MAX_EXCEPTION_MESSAGE_LENGTH 256 CACHE STRING "Control maximum length of exception messages across DLL boundaries")
#END configuration

iNCLUDE_DIRECTORIES(inc)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/static)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

SET(CWC_VERSION_MAJOR 0)
SET(CWC_VERSION_MINOR 9)
SET(CWC_VERSION_PATCH 5)
SET(CWC_LIB_DEBUG_POSTFIX "-debug")

IF(${WIN32})#assume that all compilers on Windows are compatible with Microsoft-syntax
	SET(CWC_CALL   "__cdecl")
	SET(CWC_EXPORT "__declspec(dllexport)")
ELSE()#assume that all compilers on Unix (implicitly assumed) are compatible with GCC-syntax
	SET(CWC_CALL   "__attribute__((__cdecl__))")
	SET(CWC_EXPORT "__attribute__((visibility(\"default\")))")
ENDIF()

SET(CWC_LIB_NAME cwc-${CWC_VERSION_MAJOR}.${CWC_VERSION_MINOR}.${CWC_VERSION_PATCH})

IF(${MSVC})
	IF(${MSVC_VERSION} LESS 1900)
		MESSAGE(FATAL_ERROR "Unsupported MSVC Version")
	ELSE()
		SET(CWC_LIB_NAME ${CWC_LIB_NAME}-msvc${MSVC_VERSION})
	ENDIF()
ENDIF()


SET(CWC_HOST_LIBS ${CWC_LIB_NAME})

#BEGIN find required Boost libraries
SET(REQUIRED_BOOST_LIBS filesystem system)
IF(CWC_USE_BOOST_REGEX)
	TARGET_COMPILE_DEFINITIONS(${CWC_LIB_NAME} PRIVATE CWC_CWC_USE_BOOST_REGEX)
	SET(REQUIRED_BOOST_LIBS ${REQUIRED_BOOST_LIBS} regex)
ENDIF()
IF(CWC_BUILD_TESTS)
	SET(REQUIRED_BOOST_LIBS ${REQUIRED_BOOST_LIBS} unit_test_framework)
ENDIF()
IF(REQUIRED_BOOST_LIBS)
#TODO: add option to let user select kind of boost libs?!
	SET(Boost_USE_STATIC_LIBS ON)
	SET(Boost_USE_MULTITHREADED ON)
	FIND_PACKAGE(Boost 1.58.0 REQUIRED ${REQUIRED_BOOST_LIBS})
ENDIF()
IF(CWC_USE_BOOST_REGEX)
	SET(CWC_HOST_LIBS ${CWC_HOST_LIBS} ${Boost_REGEX_LIBRARY})
ENDIF()
#END find required Boost libraries

#BEGIN CWC-library
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/cwc/cwc.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/inc/cwc/cwc.hpp @ONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/cwc/context.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/src/cwc/context.cpp @ONLY)
FILE(GLOB_RECURSE CWC_HEADERS "inc/cwc/*")
FILE(GLOB_RECURSE CWC_SOURCES "src/cwc/*")
FILE(GLOB_RECURSE CWC_GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/src/cwc/*)
ADD_LIBRARY(${CWC_LIB_NAME} STATIC ${CWC_HEADERS} ${CWC_SOURCES} ${CWC_GENERATED_SOURCES})
SET_TARGET_PROPERTIES(${CWC_LIB_NAME} PROPERTIES
	DEBUG_POSTFIX  ${CWC_LIB_DEBUG_POSTFIX}
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED ON
	COMPILE_DEFINITIONS "CWC_BUILDING;CWC_HOST"
)
#END CWC-library

#BEGIN CWCC-application + test
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/cwcc/disclaimer.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/src/cwcc/disclaimer.cpp @ONLY)
FILE(GLOB_RECURSE CWCC_SOURCES "src/cwcc/*")
FILE(GLOB_RECURSE CWCC_GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/src/cwcc/*)
ADD_EXECUTABLE(cwcc ${CWCC_SOURCES} ${CWCC_GENERATED_SOURCES})
TARGET_LINK_LIBRARIES(cwcc ${CWC_HOST_LIBS} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
SET_SOURCE_FILES_PROPERTIES(src/cwcc/parser.cpp PROPERTIES
	COMPILE_FLAGS "-O3"#NOTE: this only works as VC++ ignores unknown flags; this flag is necessary because otherwise AR from BINUTILS can't handle the this file (gets very big in debug version)
)
SET_TARGET_PROPERTIES(cwcc PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED ON
	COMPILE_DEFINITIONS CWC_HOST
)
#END CWCC-application + test

IF(CWC_BUILD_DOCUMENTATION)
	FILE(GLOB_RECURSE CWC_DOX "docs/*")
	FIND_PACKAGE(Doxygen REQUIRED)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	ADD_CUSTOM_TARGET(Documentation ALL
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_SOURCE_BINARY_DIR}
		SOURCES ${CWC_DOX}
		VERBATIM
	)
ENDIF()

IF(CWC_BUILD_TESTS)
	SET(TEST_LIBRARIES ${CWC_HOST_LIBS} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

	FILE(GLOB_RECURSE CWC_TEST_SOURCES "test/cwc/*")
	ADD_EXECUTABLE(test-cwc ${CWC_TEST_SOURCES})
	TARGET_LINK_LIBRARIES(test-cwc ${TEST_LIBRARIES})

	FILE(GLOB_RECURSE CWCC_TEST_SOURCES "test/cwcc/*")
	ADD_EXECUTABLE(test-cwcc ${CWCC_TEST_SOURCES})
	TARGET_LINK_LIBRARIES(test-cwcc ${TEST_LIBRARIES})
	SET_SOURCE_FILES_PROPERTIES(test/cwcc/shared.cpp PROPERTIES
		COMPILE_FLAGS "-O3"#NOTE: this only works as VC++ ignores unknown flags; this flag is necessary because otherwise AR from BINUTILS can't handle the this file (gets very big in debug version)
	)

	SET_TARGET_PROPERTIES(test-cwc test-cwcc PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED ON
		COMPILE_DEFINITIONS CWC_HOST
	)
ENDIF()

IF(CWC_BUILD_SAMPLES)
	FILE(GLOB_RECURSE CWC_SAMPLE_FIBONACCI_DLL_SOURCES "samples/fibonacci/dll/*.*")
	ADD_LIBRARY(sample-fibonacci-dll SHARED ${CWC_SAMPLE_FIBONACCI_DLL_SOURCES} "samples/fibonacci/cwc.sample.fibonacci.cwch")

	FILE(GLOB_RECURSE CWC_SAMPLE_FIBONACCI_EXE_SOURCES "samples/fibonacci/exe/*.*")
	ADD_EXECUTABLE(sample-fibonacci ${CWC_SAMPLE_FIBONACCI_EXE_SOURCES} "samples/fibonacci/cwc.sample.fibonacci.cwch")
	TARGET_LINK_LIBRARIES(sample-fibonacci ${CWC_HOST_LIBS})
	TARGET_COMPILE_DEFINITIONS(sample-fibonacci PRIVATE CWC_HOST)
	SET_SOURCE_FILES_PROPERTIES(samples/fibonacci/dll/cwc.sample.fibonacci.cwc PROPERTIES
		LANGUAGE CXX
		COMPILE_FLAGS "-xc++"#NOTE: this only works as VC++ ignores unknown flags; this flag is necessary because otherwise GCC won't compile this file
	)

	SET_TARGET_PROPERTIES(sample-fibonacci-dll sample-fibonacci PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED ON
	)

ENDIF()