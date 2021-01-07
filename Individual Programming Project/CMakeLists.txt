cmake_minimum_required(VERSION 3.0)
project(Gamma C)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-g")

set(SOURCE_FILES
    src/auxiliary_structs.h
    src/gamma.c
    src/gamma.h
    src/batch_mode.c
    src/batch_mode.h
    src/interactive_mode.c
    src/interactive_mode.h
    src/parsing.c
    src/parsing.h
    src/terminal_management.c
    src/terminal_management.h
    src/gamma_main.c)

add_executable(gamma ${SOURCE_FILES})

set(TEST_SOURCE_FILES
    src/auxiliary_structs.h
    src/gamma.c
    src/gamma.h
    src/gamma_test.c)

add_executable(test EXCLUDE_FROM_ALL ${TEST_SOURCE_FILES})

set_target_properties(test PROPERTIES OUTPUT_NAME gamma_test)


find_package(Doxygen)
if (DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
endif (DOXYGEN_FOUND)
