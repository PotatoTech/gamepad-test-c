# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2021 PotatoTech

cmake_minimum_required(VERSION 3.13)

project(gamepad-test-c LANGUAGES C)

# Require out-of-source builds.
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    message(
        FATAL_ERROR
            "In-source builds are not allowed. Please make a build subdirectory. Feel free to delete CMakeCache.txt and CMakeFiles."
    )
endif()

add_executable(${PROJECT_NAME} "")

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES C_STANDARD 99
               C_STANDARD_REQUIRED ON
               C_EXTENSIONS OFF
               RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

foreach(output_config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER "${output_config}" output_config)
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${output_config}
                                                     "${PROJECT_BINARY_DIR}/bin")
endforeach()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/submodules/sdl2-cmake-modules)
find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::Main)

option(ARGPARSE_SHARED "" OFF)
add_subdirectory(submodules/argparse)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/submodules/argparse)
target_link_libraries(${PROJECT_NAME} PRIVATE argparse)

target_sources(${PROJECT_NAME} PRIVATE src/main.c)

# Set compiler warnings.
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
elseif(CMAKE_C_COMPILER_ID MATCHES "^GNU$|Clang$")
    target_compile_options(
        ${PROJECT_NAME}
        PRIVATE -Wall
                -Wextra
                -Wpedantic
                -Wshadow
                -Wconversion
                -Wsign-conversion
                -Werror=implicit-function-declaration
                -Werror=return-type)
endif()

# Enable interprocedural optimization for release builds if supported by the compiler.
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/GL>)
        target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/LTCG>)
    else()
        string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_build_type)
        if(uppercase_build_type STREQUAL "RELEASE")
            set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
        endif()
    endif()
endif()

get_target_property(runtime_output_dir ${PROJECT_NAME} RUNTIME_OUTPUT_DIRECTORY)
if(NOT runtime_output_dir)
    message(FATAL_ERROR "The runtime output directory for ${PROJECT_NAME} is not set.")
endif()

# Create the bin directory at build time.
add_custom_command(
    TARGET ${PROJECT_NAME}
    PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E make_directory ${runtime_output_dir})

# Copy the game controller mappings into bin at build time.
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E copy
        ${PROJECT_SOURCE_DIR}/submodules/SDL_GameControllerDB/gamecontrollerdb.txt
        ${runtime_output_dir}
    COMMENT "Copying game controller mappings")
