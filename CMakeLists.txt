cmake_minimum_required(VERSION 3.7)

project(vsgwx
        VERSION 0.1.0
        DESCRIPTION "wxWidgets integration with Vulkan Scene Graph"
        LANGUAGES CXX)

find_package(vsg 1.0.0 REQUIRED)
find_package(vsgXchange) # only used by examples

vsg_setup_dir_vars()
vsg_setup_build_vars()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(vsgwx_SOVERSION 0)
set(vsgwx_RELEASE_CANDIDATE 0)

set(VSGWX_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "Root source directory of vsgwx")
set(VSGWX_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "Root binary directory of vsgwx")

vsg_add_target_uninstall()

add_subdirectory(src/vsgwx)
add_subdirectory(examples)

vsg_add_feature_summary()
