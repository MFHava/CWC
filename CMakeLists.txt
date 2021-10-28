
#          Copyright Michael Florian Hava.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.13)
project(CWC)

option(CWC_BUILD_TESTS "Build tests" OFF)
option(CWC_BUILD_SAMPLES "Build samples" OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/static)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE CWC_HDR "inc/cwc/*")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/inc/cwc FILES ${CWC_HDR})
file(GLOB_RECURSE CWC_SRC "src/cwc/*")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/cwc FILES ${CWC_SRC})
add_library(cwc STATIC ${CWC_HDR} ${CWC_SRC})
target_include_directories(cwc PUBLIC "inc")
target_link_libraries(cwc PUBLIC ${CMAKE_DL_LIBS})
set_target_properties(cwc PROPERTIES FOLDER "C++ with Components" POSITION_INDEPENDENT_CODE ON CXX_VISIBILITY_PRESET hidden)

find_package(Boost 1.59 REQUIRED program_options)

file(GLOB_RECURSE CWCC "src/cwcc/*")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/cwcc FILES ${CWCC})
add_executable(cwcc ${CWCC})
target_link_libraries(cwcc cwc Boost::program_options)
set_target_properties(cwcc PROPERTIES FOLDER "C++ with Components")

find_package(Doxygen 1.8.2) #Doxygen 1.8.2 is the first version to support C++11
if(Doxygen_FOUND)
	set(DOXYGEN_PROJECT_NAME "C++ with Components (${CWC_VERSION})")
	set(DOXYGEN_PROJECT_BRIEF "A portable extension of C++ to support component oriented software enginnering.")
	set(DOXYGEN_OUTPUT_DIRECTORY "docs")
	set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
	set(DOXYGEN_SHOW_USED_FILES NO)
	set(DOXYGEN_SHOW_FILES NO)
	set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
	set(DOXYGEN_FILE_PATTERNS "*.hpp;*.dox")
	set(DOXYGEN_EXCLUDE_SYMBOLS "cwc::internal;cwc::internal::*;cwc_*")
	set(DOXYGEN_GENERATE_TREEVIEW YES)
	set(DOXYGEN_COLLABORATION_GRAPH NO)
	doxygen_add_docs(CWC-Documentation "inc;docs")
	set_target_properties(CWC-Documentation PROPERTIES FOLDER "C++ with Components")
endif()

if(CWC_BUILD_TESTS)
	find_package(Boost 1.59 REQUIRED unit_test_framework)
	file(GLOB_RECURSE CWC_TEST "test/cwc/*")
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/test/cwc FILES ${CWC_TEST})
	add_executable(test-cwc ${CWC} ${CWC_TEST}) #including CWC sources here to have them included in IDEs
	target_link_libraries(test-cwc PRIVATE cwc Boost::unit_test_framework)
	set_target_properties(test-cwc PROPERTIES FOLDER "C++ with Components")

	file(GLOB_RECURSE CWCC_TEST "test/cwcc/*")
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/test/cwcc FILES ${CWCC_TEST})
	add_executable(test-cwcc ${CWCC_TEST})
	target_link_libraries(test-cwcc PRIVATE cwc Boost::unit_test_framework)
	set_target_properties(test-cwcc PROPERTIES FOLDER "C++ with Components")
endif()

if(CWC_BUILD_SAMPLES)
	foreach(sample fibonacci)
		file(GLOB_RECURSE DLL "samples/${sample}/dll/*.*")
		source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/samples/${sample}/dll FILES ${DLL})
		add_library(sample-${sample}-dll SHARED ${DLL} "samples/${sample}/cwc.sample.${sample}.cwch")
		target_link_libraries(sample-${sample}-dll PRIVATE cwc)
		set_target_properties(sample-${sample}-dll PROPERTIES OUTPUT_NAME sample-${sample} FOLDER "C++ with Components/Samples" CXX_VISIBILITY_PRESET hidden)

		file(GLOB_RECURSE EXE "samples/${sample}/exe/*.*")
		source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/samples/${sample}/exe FILES ${EXE})
		add_executable(sample-${sample}-exe ${EXE} "samples/${sample}/cwc.sample.${sample}.cwch")
		target_link_libraries(sample-${sample}-exe PRIVATE cwc)
		set_target_properties(sample-${sample}-exe PROPERTIES OUTPUT_NAME sample-${sample} FOLDER "C++ with Components/Samples")
	endforeach(sample)
endif()
