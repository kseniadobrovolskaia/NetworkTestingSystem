cmake_minimum_required(VERSION 3.14)

project(GUI_Window)
set(LIB_NAME GUI_Window)

add_compile_options(-std=c++20 -fPIC)

find_package(PkgConfig)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

include_directories(${CMAKE_SOURCE_DIR}/GUI/include ${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})

set(SOURCE_LIB lib/GUI_Window.cpp)

add_library(GUI_Window STATIC ${SOURCE_LIB})

add_executable(showWindow main.cpp)
target_link_libraries(showWindow GUI_Window ${GTKMM_LIBRARIES})

