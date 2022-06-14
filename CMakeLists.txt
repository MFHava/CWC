
#          Copyright Michael Florian Hava.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.13)
project(CWC)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/static)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


file(GLOB_RECURSE CWC_HDR "inc/cwc/*")
source_group("inc" FILES ${CWC_HDR})
file(GLOB_RECURSE CWC_SRC "src/cwc/*")
source_group("src" FILES ${CWC_SRC})
add_library(cwc STATIC ${CWC_HDR} ${CWC_SRC})
target_include_directories(cwc PUBLIC "inc")
target_link_libraries(cwc PUBLIC ${CMAKE_DL_LIBS})
set_target_properties(cwc PROPERTIES FOLDER "C++ with Components" POSITION_INDEPENDENT_CODE ON CXX_VISIBILITY_PRESET hidden)


file(GLOB_RECURSE CWCC_LIB "src/libcwcc/*")
source_group("" FILES ${CWCC_LIB})
add_library(libcwcc OBJECT ${CWCC_LIB})
target_include_directories(libcwcc PUBLIC "src/libcwcc")
set_target_properties(libcwcc PROPERTIES FOLDER "C++ with Components")


file(GLOB_RECURSE CWCC "src/cwcc/*")
source_group("" FILES ${CWCC})
add_executable(cwcc ${CWCC})
target_link_libraries(cwcc PRIVATE libcwcc)
set_target_properties(cwcc PROPERTIES FOLDER "C++ with Components")


function(invoke_cwcc cwc_file cwch_file)
	add_custom_command(OUTPUT ${cwch_file} COMMAND cwcc ARGS ${cwc_file} ${cwch_file} DEPENDS ${cwc_file} cwcc VERBATIM)
endfunction()


find_package(Doxygen 1.8.2) #Doxygen 1.8.2 is the first version to support C++11
if(Doxygen_FOUND)
	set(DOXYGEN_PROJECT_NAME "C++ with Components")
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


option(CWC_BUILD_SAMPLES "Build samples" OFF)
if(CWC_BUILD_SAMPLES)
	foreach(sample fibonacci)
		file(GLOB_RECURSE CWC_SAMPLE "samples/${sample}/*")
		source_group("" FILES ${CWC_SAMPLE})

		invoke_cwcc("${CMAKE_CURRENT_SOURCE_DIR}/samples/${sample}/${sample}.cwc" "${sample}.cwch")

		add_library(sample-${sample}-dll SHARED ${CWC_SAMPLE} "${sample}.cwch")
		target_compile_definitions(sample-${sample}-dll PRIVATE CWC_SAMPLE_DLL)
		target_include_directories(sample-${sample}-dll PRIVATE ${CMAKE_BINARY_DIR})
		target_link_libraries(sample-${sample}-dll PRIVATE cwc)
		set_target_properties(sample-${sample}-dll PROPERTIES OUTPUT_NAME sample-${sample} FOLDER "C++ with Components/Samples" CXX_VISIBILITY_PRESET hidden)

		add_executable(sample-${sample}-exe ${CWC_SAMPLE} "${sample}.cwch")
		target_include_directories(sample-${sample}-exe PRIVATE ${CMAKE_BINARY_DIR})
		target_link_libraries(sample-${sample}-exe PRIVATE cwc)
		set_target_properties(sample-${sample}-exe PROPERTIES OUTPUT_NAME sample-${sample} FOLDER "C++ with Components/Samples")
	endforeach(sample)
endif()


option(CWC_BUILD_TESTS "Build unit tests" OFF)
if(CWC_BUILD_TESTS)
	find_package(Catch2 CONFIG REQUIRED)

	file(GLOB_RECURSE CWC_TEST "test/cwc/*")
	source_group("" FILES ${CWC_TEST})

	invoke_cwcc("${CMAKE_CURRENT_SOURCE_DIR}/test/cwc/test.cwc" "test.cwch")

	add_library(test-cwc-dll SHARED ${CWC_TEST} "test.cwch")
	target_compile_definitions(test-cwc-dll PRIVATE CWC_TEST_DLL)
	target_include_directories(test-cwc-dll PRIVATE ${CMAKE_BINARY_DIR})
	target_link_libraries(test-cwc-dll PRIVATE cwc Catch2::Catch2 Catch2::Catch2WithMain)
	set_target_properties(test-cwc-dll PROPERTIES OUTPUT_NAME test-cwc FOLDER "C++ with Components/Tests" CXX_VISIBILITY_PRESET hidden)

	add_executable(test-cwc-exe ${CWC_TEST} "test.cwch")
	target_include_directories(test-cwc-exe PRIVATE ${CMAKE_BINARY_DIR})
	target_link_libraries(test-cwc-exe PRIVATE cwc Catch2::Catch2 Catch2::Catch2WithMain)
	set_target_properties(test-cwc-exe PROPERTIES OUTPUT_NAME test-cwc FOLDER "C++ with Components/Tests")


	file(GLOB_RECURSE CWCC_TEST "test/cwcc/*")
	source_group("" FILES ${CWCC_TEST})

	add_executable(test-cwcc ${CWCC_TEST})
	target_link_libraries(test-cwcc PRIVATE libcwcc Catch2::Catch2 Catch2::Catch2WithMain)
	set_target_properties(test-cwcc PROPERTIES FOLDER "C++ with Components/Tests")
endif()
