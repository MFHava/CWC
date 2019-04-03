
#          Copyright Michael Florian Hava 2012.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.13)
project(CWC)

option(CWC_BUILD_TESTS "Build tests" OFF)
option(CWC_BUILD_SAMPLES "Build samples" OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(external/PTL)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/static)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(cwc_flags INTERFACE)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(cwc_flags INTERFACE -fvisibility=hidden) #hide all symbols not explicitly exported
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_definitions(cwc_flags INTERFACE _CRT_SECURE_NO_WARNINGS)
endif()

add_library(cwc INTERFACE)
target_include_directories(cwc INTERFACE "inc")
target_link_libraries(cwc INTERFACE cwc_flags ptl ${CMAKE_DL_LIBS})

find_package(Boost 1.59 REQUIRED program_options)

file(GLOB_RECURSE CWCC "src/cwcc/*")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/cwcc FILES ${CWCC})
add_executable(cwcc ${CWCC})
target_link_libraries(cwcc cwc Boost::program_options)
set_target_properties(cwcc PROPERTIES FOLDER "C++ with Components")

find_package(Doxygen)
if(Doxygen_FOUND)
	set(DOXYGEN_PROJECT_NAME "C++ with Components (${CWC_VERSION})")
	set(DOXYGEN_PROJECT_BRIEF "A portable extension of C++ to support component oriented software enginnering.")
	set(DOXYGEN_OUTPUT_DIRECTORY "docs")
	set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
	set(DOXYGEN_SHOW_USED_FILES NO)
	set(DOXYGEN_SHOW_FILES NO)
	set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
	set(DOXYGEN_FILE_PATTERNS "*.hpp;*.dox")
	set(DOXYGEN_EXCLUDE_SYMBOLS "cwc::internal;cwc::internal::*;cwc_*;cwc::TL;cwc::TL::*")
	set(DOXYGEN_GENERATE_TREEVIEW YES)
	set(DOXYGEN_MACRO_EXPANSION YES)
	set(DOXYGEN_PREDEFINED "CWC_HOST")
	set(DOXYGEN_COLLABORATION_GRAPH NO)
	doxygen_add_docs(CWC-Documentation "inc;docs")
	set_target_properties(CWC-Documentation PROPERTIES FOLDER "C++ with Components")
endif()

if(CWC_BUILD_TESTS)
	find_package(Boost 1.59 REQUIRED unit_test_framework)
	file(GLOB_RECURSE CWC "inc/*")
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/inc FILES ${CWC})
	file(GLOB_RECURSE CWC_TEST "test/cwc/*")
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/test/cwc FILES ${CWC_TEST})
	add_executable(test-cwc ${CWC} ${CWC_TEST}) #including CWC sources here to have them included in IDEs
	target_compile_definitions(test-cwc PRIVATE BOOST_TEST_DYN_LINK) #assuming that system uses dynamic Boost
	target_link_libraries(test-cwc cwc Boost::unit_test_framework)
	set_target_properties(test-cwc PROPERTIES FOLDER "C++ with Components")

	file(GLOB_RECURSE CWCC_TEST "test/cwcc/*")
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/test/cwcc FILES ${CWCC_TEST})
	add_executable(test-cwcc ${CWCC_TEST})
	target_compile_definitions(test-cwcc PRIVATE BOOST_TEST_DYN_LINK) #assuming that system uses dynamic Boost
	target_link_libraries(test-cwcc cwc Boost::unit_test_framework)
	set_target_properties(test-cwcc PROPERTIES FOLDER "C++ with Components")
endif()

if(CWC_BUILD_SAMPLES)
	foreach(sample fibonacci)
		file(GLOB_RECURSE DLL "samples/${sample}/dll/*.*")
		source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/samples/${sample}/dll FILES ${DLL})
		add_library(sample-${sample}-dll SHARED ${DLL} "samples/${sample}/cwc.sample.${sample}.cwch")
		target_link_libraries(sample-${sample}-dll cwc)
		set_target_properties(sample-${sample}-dll PROPERTIES OUTPUT_NAME sample-${sample} FOLDER "C++ with Components/Samples")

		file(GLOB_RECURSE EXE "samples/${sample}/exe/*.*")
		source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/samples/${sample}/exe FILES ${EXE})
		add_executable(sample-${sample}-exe ${EXE} "samples/${sample}/cwc.sample.${sample}.cwch")
		target_link_libraries(sample-${sample}-exe cwc)
		set_target_properties(sample-${sample}-exe PROPERTIES OUTPUT_NAME sample-${sample} FOLDER "C++ with Components/Samples")
	endforeach(sample)
endif()
