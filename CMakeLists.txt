
#          Copyright Michael Florian Hava.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.23)
project(CWC)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(CWCC_GENERATED_DIRECTORY "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${CWCC_GENERATED_DIRECTORY})


add_library(flags INTERFACE)
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		target_compile_options(flags INTERFACE -Wall -Wextra -Wpedantic -Wconversion)
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		target_compile_options(flags INTERFACE /W4 /JMC /MP /permissive- /bigobj
			/wd4456 # ignore local shadowing
			/wd4457 # ignore function parameter shadowing
			/wd4458 # ignore class member shadowing
			/wd4459 # ignore global shadowing
		)
	endif()


add_library(cwc STATIC)
	file(GLOB_RECURSE CWC_HDR "inc/cwc/*")
		source_group("inc" FILES ${CWC_HDR})
	file(GLOB_RECURSE CWC_SRC "src/cwc/*")
		source_group("src" FILES ${CWC_SRC})
	target_sources(cwc PRIVATE ${CWC_HDR} ${CWC_SRC})
	target_include_directories(cwc PUBLIC "inc")
	target_link_libraries(cwc PUBLIC ${CMAKE_DL_LIBS} PRIVATE flags)
	set_target_properties(cwc PROPERTIES POSITION_INDEPENDENT_CODE ON CXX_VISIBILITY_PRESET hidden)


add_library(libcwcc OBJECT)
	file(GLOB_RECURSE CWCC_LIB "src/libcwcc/*")
		source_group("" FILES ${CWCC_LIB})
	target_sources(libcwcc PRIVATE ${CWCC_LIB})
	target_include_directories(libcwcc PUBLIC "src/libcwcc")
	target_link_libraries(libcwcc PUBLIC flags)
	set_target_properties(libcwcc PROPERTIES FOLDER "CWCC")


add_executable(cwcc)
	file(GLOB_RECURSE CWCC "src/cwcc/*")
		source_group("" FILES ${CWCC})
	target_sources(cwcc PRIVATE ${CWCC})
	target_link_libraries(cwcc PRIVATE libcwcc)
	set_target_properties(cwcc PROPERTIES FOLDER "CWCC")


function(invoke_cwcc cwc_file cwch_file)
	add_custom_command(OUTPUT ${cwch_file} COMMAND cwcc ARGS ${cwc_file} ${cwch_file} DEPENDS ${cwc_file} cwcc VERBATIM)
	source_group("generated" FILES ${cwch_file})
endfunction()


find_package(Doxygen 1.9.1)
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
	doxygen_add_docs(Documentation "inc;docs")
endif()


option(CWC_BUILD_SAMPLES "Build samples" OFF)
if(CWC_BUILD_SAMPLES)
	foreach(sample fibonacci)
		add_library(sample-${sample} INTERFACE)
			file(GLOB_RECURSE CWC_SAMPLE "samples/${sample}/*")
				source_group("" FILES ${CWC_SAMPLE})
			invoke_cwcc("${CMAKE_CURRENT_SOURCE_DIR}/samples/${sample}/${sample}.cwc" "${CWCC_GENERATED_DIRECTORY}/${sample}.cwch")
			target_sources(sample-${sample} INTERFACE ${CWC_SAMPLE} "${CWCC_GENERATED_DIRECTORY}/${sample}.cwch")
			target_include_directories(sample-${sample} INTERFACE ${CWCC_GENERATED_DIRECTORY})
			target_link_libraries(sample-${sample} INTERFACE cwc flags)
			add_library(sample-${sample}-dll SHARED)
				target_compile_definitions(sample-${sample}-dll PRIVATE CWC_SAMPLE_DLL)
				target_link_libraries(sample-${sample}-dll PRIVATE sample-${sample})
				set_target_properties(sample-${sample}-dll PROPERTIES OUTPUT_NAME sample-${sample} FOLDER "Samples" CXX_VISIBILITY_PRESET hidden)
			add_executable(sample-${sample}-exe)
				target_link_libraries(sample-${sample}-exe PRIVATE sample-${sample})
				set_target_properties(sample-${sample}-exe PROPERTIES OUTPUT_NAME sample-${sample} FOLDER "Samples")
	endforeach(sample)
endif()


option(CWC_BUILD_TESTS "Build unit tests" OFF)
if(CWC_BUILD_TESTS)
	find_package(Catch2 CONFIG REQUIRED)
	enable_testing()


	add_library(test-cwc INTERFACE)
		file(GLOB_RECURSE CWC_TEST "test/cwc/*")
			source_group("" FILES ${CWC_TEST})
		invoke_cwcc("${CMAKE_CURRENT_SOURCE_DIR}/test/cwc/test.cwc" "${CWCC_GENERATED_DIRECTORY}/test.cwch")
		target_sources(test-cwc INTERFACE ${CWC_TEST} "${CWCC_GENERATED_DIRECTORY}/test.cwch")
		target_include_directories(test-cwc INTERFACE ${CWCC_GENERATED_DIRECTORY})
		target_link_libraries(test-cwc INTERFACE cwc Catch2::Catch2 Catch2::Catch2WithMain flags)
		add_library(test-cwc-dll SHARED)
			target_compile_definitions(test-cwc-dll PRIVATE CWC_TEST_DLL)
			target_link_libraries(test-cwc-dll PRIVATE test-cwc)
			set_target_properties(test-cwc-dll PROPERTIES OUTPUT_NAME test-cwc FOLDER "Tests" CXX_VISIBILITY_PRESET hidden)
		add_executable(test-cwc-exe)
			target_link_libraries(test-cwc-exe PRIVATE test-cwc)
			set_target_properties(test-cwc-exe PROPERTIES OUTPUT_NAME test-cwc FOLDER "Tests")
			add_test(NAME cwc COMMAND test-cwc-exe)


	add_executable(test-cwcc)
		file(GLOB_RECURSE CWCC_TEST "test/cwcc/*")
			source_group("" FILES ${CWCC_TEST})
		target_sources(test-cwcc PRIVATE ${CWCC_TEST})
		target_link_libraries(test-cwcc PRIVATE libcwcc Catch2::Catch2 Catch2::Catch2WithMain)
		set_target_properties(test-cwcc PROPERTIES FOLDER "Tests")
		add_test(NAME cwcc COMMAND test-cwcc)
endif()
