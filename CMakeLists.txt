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
    "${S}/src")

  add_custom_command(
    OUTPUT  "${B}/src/fod_lexer.c"
    DEPENDS "${S}/src/fod_lexer.rl"
            "${B}/src/fod_parser.h"
    COMMAND cmake -E copy "${S}/src/fod_lexer.rl" "${B}/src/fod_lexer.rl"
    COMMAND ragel -C "${B}/src/fod_lexer.rl" 
    COMMENT "Generating lexer code with Ragel")
  
add_custom_command(
    OUTPUT  "${B}/src/fod_parser.h"
            "${B}/src/fod_parser.c"
    DEPENDS "${S}/src/fod_parser.y"
    COMMAND cmake -E copy "${S}/src/fod_parser.y" "${B}/src/fod_parser.y"
    COMMAND lemon "${B}/src/fod_parser.y"
    COMMENT "Generating parser code with LEMON")

  add_library(fod STATIC
    "${B}/src/fod_lexer.c"
    "${B}/src/fod_parser.c"
    )
# add_library(SHARED)

# custom_command(ragel)
# custom_command(lemon)
# 