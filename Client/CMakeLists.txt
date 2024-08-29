cmake_minimum_required(VERSION 3.14)

project(Client)
set(EXEC_NAME Client)

find_package(PkgConfig)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

link_directories(${GTKMM_LIBRARY_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/Client/include
                    ${CMAKE_SOURCE_DIR}/GUI/include
	            ${GTKMM_INCLUDE_DIRS})

set(SOURCE_LIB lib/Client.cpp)

add_executable(${EXEC_NAME} ${SOURCE_LIB})
target_link_libraries(${EXEC_NAME} GUI_Window ${GTKMM_LIBRARIES})

