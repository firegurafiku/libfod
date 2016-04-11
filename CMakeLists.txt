cmake_minimum_required(VERSION 2.6)

project(fod C)

# Just shortcuts. They're short, nice and distinctive, so any path must be
# explicitly specified as for source directory or build directory (with some
# small exceptions like for 'install' command).
set(S "${CMAKE_CURRENT_SOURCE_DIR}")
set(B "${CMAKE_CURRENT_BINARY_DIR}")

# Thirdparty libraries lookup.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${S}/cmake")
find_package(OpenCL REQUIRED)
find_package(PythonInterp REQUIRED)
find_program(LEMON lemon)
find_program(RAGEL ragel)

# Build options.
option(FOD_CONFIG_UNIT_TESTS "Build unit-tests for library")
option(FOD_CONFIG_CPPCHECK   "Check library sources with CppCheck")
option(FOD_CONFIG_VALGRIND   "Check library unit tests with Valgrind")
option(FOD_INSTALL_SOURCES   "Install source files too")

if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")

    # These are the prefered GCC options for *developer* environment. They try
    # to enable as much of useful warnings as possible while trying to keep
    # “signal-noise” ratio in compiler output still acceptable. Please ensure
    # that your code builds with these settings without warnings BEFORE merging
    # it into master branch.
    # Originally taken from http://stackoverflow.com/a/1667114/1447225.
    set(CMAKE_C_FLAGS_DEBUG
        "${CMAKE_C_FLAGS_DEBUG}"
        -std=c99 -O0 -ggdb3 -pg --coverage
        -Wall
        -Wextra
        -Wpedantic
        -Wbad-function-cast
        -Wcast-align
        -Wcast-qual
        -Wconversion
        -Wfloat-equal
        -Wformat=2
        -Winline
        -Wjump-misses-init
        -Wlogical-op
        # -Wmissing-declarations
        -Wmissing-field-initializers
        -Wmissing-include-dirs
        # -Wmissing-prototypes
        -Wnested-externs
        -Wold-style-definition
        -Woverlength-strings
        -Wpointer-arith
        -Wredundant-decls
        -Wshadow
        -Wstack-protector
        -Wstrict-aliasing=2
        -Wstrict-prototypes
        -Wswitch-default
        -Wundef
        -Wunreachable-code
        #-Wunsuffixed-float-constants
        -Wunused
        # -Wuseless-cast
        -Wwrite-strings
        -ffloat-store
        -fno-common
        -fstrict-aliasing
        -ftree-vrp)

    # Flattern options list properly.
    string(REPLACE ";" " " CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
endif()

include_directories(
    "${S}/include"
    "${S}/src"
    "${B}/src")

add_custom_command(
    OUTPUT  "${B}/src/fod_lexer.c"
    DEPENDS "${S}/src/fod_lexer.rl"
            "${B}/src/fod_parser_gen.h"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${B}/src"
    COMMAND "${CMAKE_COMMAND}" -E copy
	        "${S}/src/fod_lexer.rl" "${B}/src/fod_lexer.rl"
    COMMAND "${RAGEL}" -C "${B}/src/fod_lexer.rl" 
    COMMENT "Generating lexer code with Ragel")
  
add_custom_command(
    OUTPUT  "${B}/src/fod_parser_gen.h"
            "${B}/src/fod_parser_gen.c"
    DEPENDS "${S}/src/fod_parser_gen.y"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${B}/src"
    COMMAND "${CMAKE_COMMAND}" -E copy
                "${S}/src/fod_parser_gen.y" "${B}/src/fod_parser_gen.y"
    COMMAND "${LEMON}" "${B}/src/fod_parser_gen.y"
    COMMENT "Generating parser code with LEMON")

add_library(fod STATIC
        "${S}/src/fod_devfind.c"
    "${B}/src/fod_lexer.c"
    "${S}/src/fod_parser.c"
    "${B}/src/fod_parser_gen.c"
    "${S}/src/fod_common.c")

# -- unit tests --
if(FOD_CONFIG_UNIT_TESTS)
    find_package(PythonInterp REQUIRED)
    find_package(CMocka       REQUIRED)

    add_executable(test_fod
        "${S}/test/main_cmocka.c"
        "${B}/test/generated_tests.h"
	"${S}/test/test_common.c"
	"${S}/test/test_evaltoken.c"
        "${S}/test/test_lexer.c"
        "${S}/test/test_parser.c"
	)

    target_include_directories(test_fod PRIVATE "${B}/test")
    target_link_libraries(test_fod fod "${CMOCKA_LIBRARY}" ${OpenCL_LIBRARIES})
    
    # Gather all '*.c' files from 'rvlmfdtd-test' target into TEST_SOURCES.
    get_target_property(TEST_SOURCES_TMP test_fod SOURCES)
    set(TEST_SOURCES)
    foreach(ITEM ${TEST_SOURCES_TMP})
        string(REGEX MATCH ".c$" ITEM_MATCH "${ITEM}")
        if(ITEM_MATCH)
            list(APPEND TEST_SOURCES "${ITEM}")
        endif()
    endforeach()

    add_custom_command(
        OUTPUT  "${B}/test/generated_tests.h"
        DEPENDS "${S}/tools/generate_test_registry.py" ${TEST_SOURCES}
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${B}/test"
        COMMAND "${PYTHON_EXECUTABLE}"
                "${S}/tools/generate_test_registry.py"
                    --output "${B}/test/generated_tests.h"
                ${TEST_SOURCES}
        COMMENT "Generating CMocka registry header file")

    if (FOD_CONFIG_VALGRIND)
        add_test(test_fod valgrind ./test_fod)
    else()
        add_test(test_fod test_fod)
    endif()
endif()