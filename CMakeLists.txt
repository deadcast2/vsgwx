cmake_minimum_required(VERSION 3.7)

project(vsgwx
        VERSION 0.0.0
        DESCRIPTION "wxWidgets with VSG"
        LANGUAGES CXX)

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)

INCLUDE_DIRECTORIES(${GTK3_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})

find_package(vsg 0.4.0 REQUIRED)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES main.cpp)

add_executable(vsgwx ${SOURCES})

target_link_libraries(vsgwx vsg::vsg ${wxWidgets_LIBRARIES} ${GTK3_LIBRARIES})
